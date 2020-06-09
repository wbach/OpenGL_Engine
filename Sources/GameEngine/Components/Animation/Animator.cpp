#include "Animator.h"

#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
using namespace Animation;

namespace Components
{
void JointData::updateBufferTransform()
{
    if (buffer)
        updateBufferTransform(rootJoint);
}
void JointData::updateBufferTransform(Animation::Joint& joint)
{
    if (joint.id < MAX_BONES)
    {
        buffer->GetData().bonesTransforms[joint.id] = joint.animatedTransform;
    }
    for (auto& childJoint : joint.children)
    {
        updateBufferTransform(childJoint);
    }
}

ComponentsType Animator::type = ComponentsType::Animator;

Animator::Animator(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Animator, componentContext, gameObject)
    , currentTime_(0.f)
    , animationSpeed_(1.f)
    , changeAnimTime_(0.25f)
    , current_("DefaultAnimationClip")
    , changeAnim(false)
{
}

void Animator::CleanUp()
{
    for (auto& jd : meshRootJoints_)
    {
        if (jd.buffer and jd.buffer->GetGraphicsObjectId())
            componentContext_.gpuResourceLoader_.AddObjectToRelease(std::move(jd.buffer));
    }
    meshRootJoints_.clear();
}
void Animator::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Update, std::bind(&Animator::Update, this));
    RegisterFunction(FunctionType::Awake, std::bind(&Animator::GetSkeletonAndAnimations, this));
}

Animator& Animator::SetAnimation(const std::string& name)
{
    current_     = name;
    currentTime_ = 0.f;

    return *this;
}
const std::string& Animator::GetCurrentAnimationName() const
{
    return current_;
}
const GraphicsApi::ID& Animator::getPerPoseBufferId(uint32 i) const
{
    return meshRootJoints_[i].buffer->GetGraphicsObjectId();
}
void Animator::ChangeAnimation(const std::string& name)
{
    auto& endFrames = animationClips_[name].GetFrames();
    if (endFrames.empty())
        return;

    endChangeAnimPose = endFrames[0];

    nextClip_              = name;
    changeAnim             = true;
    currentChangeAnimTime_ = 0;

    startChaneAnimPose = KeyFrame();
    for (const auto& p : calculateCurrentAnimationPose())
        startChaneAnimPose.transforms[p.first] = GetJointTransform(p.second);
}
void Animator::GetSkeletonAndAnimations()
{
    auto renderer = thisObject_.GetComponent<RendererComponent>();

    if (renderer == nullptr)
        return;

    auto model = renderer->GetModelWrapper().Get(GameEngine::L1);

    if (model)
    {
        for (auto& mesh : model->GetMeshes())
        {
            meshRootJoints_.emplace_back();
            auto& jd = meshRootJoints_.back();

            jd.rootJoint = mesh.getRootJoint();
            jd.buffer    = std::make_unique<BufferObject<PerPoseUpdate>>(componentContext_.graphicsApi_,
                                                                      PER_POSE_UPDATE_BIND_LOCATION);
        }

        for (auto& jd : meshRootJoints_)
        {
            auto& bufferData = jd.buffer->GetData();
            for (size_t i = 0; i < MAX_BONES; ++i)
            {
                bufferData.bonesTransforms[i] = mat4(1.f);
            }
            componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(*jd.buffer);
        }

        animationClips_ = model->animationClips_;

        if (not animationClips_.empty())
        {
            if (not animationClips_.count(current_))
            {
                current_ = animationClips_.begin()->first;
            }
        }
    }
}
void Animator::updateShaderBuffers()
{
    for (auto& jd : meshRootJoints_)
    {
        jd.updateBufferTransform();
        componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(*jd.buffer);
    }
}
void Animator::ChangeAnimState()
{
    currentChangeAnimTime_ += (1.f / changeAnimTime_) * componentContext_.time_.deltaTime * animationSpeed_;

    if (currentChangeAnimTime_ > 1.f)
    {
        changeAnim             = false;
        current_               = nextClip_;
        currentTime_           = 0.f;
        currentChangeAnimTime_ = 1.f;
    }
    auto pos = interpolatePoses(startChaneAnimPose, endChangeAnimPose, currentChangeAnimTime_);
    applyPoseToJoints(pos);
}
void Animator::applyPoseToJoints(const Pose& pose)
{
    for (auto& jd : meshRootJoints_)
    {
        applyPoseToJoints(pose, jd.rootJoint, mat4(1.f));
    }
    updateShaderBuffers();
}
bool Animator::IsReady()
{
    return (not meshRootJoints_.empty() && !current_.empty() && animationClips_.count(current_) != 0);
}
void Animator::Update()
{
    if (!IsReady())
        return;

    if (changeAnim)
    {
        ChangeAnimState();
        return;
    }

    increaseAnimationTime();
    auto currentPose = calculateCurrentAnimationPose();
    applyPoseToJoints(currentPose);
}
void Animator::increaseAnimationTime()
{
    auto animationLength = animationClips_[current_].GetLength();

    currentTime_ += componentContext_.time_.deltaTime * animationSpeed_;
    if (currentTime_ > animationLength)
        currentTime_ = fmodf(currentTime_, animationLength);
    if (currentTime_ < 0)
        currentTime_ = animationLength + currentTime_;
}

Pose Animator::calculateCurrentAnimationPose()
{
    auto frames       = getPreviousAndNextFrames();
    float progression = calculateProgression(frames.first, frames.second);
    return interpolatePoses(frames.first, frames.second, progression);
}

Pose Animator::interpolatePoses(const KeyFrame& previousFrame, const KeyFrame& nextFrame, float progression)
{
    Pose currentPose;
    for (const auto& pair : previousFrame.transforms)
    {
        auto& jointName = pair.first;
        if (previousFrame.transforms.count(jointName) == 0 || nextFrame.transforms.count(jointName) == 0)
            continue;

        JointTransform previousTransform = previousFrame.transforms.at(jointName);
        JointTransform nextTransform     = nextFrame.transforms.at(jointName);
        JointTransform currentTransform  = Interpolate(previousTransform, nextTransform, progression);
        currentPose[jointName]           = GetLocalTransform(currentTransform);
    }
    return currentPose;
}

std::pair<KeyFrame, KeyFrame> Animator::getPreviousAndNextFrames()
{
    const auto& allFrames = animationClips_[current_].GetFrames();

    KeyFrame previousFrame = allFrames[0];
    KeyFrame nextFrame     = allFrames[0];
    for (uint32 i = 1; i < allFrames.size(); i++)
    {
        nextFrame = allFrames[i];
        if (nextFrame.timeStamp > currentTime_)
            break;

        previousFrame = allFrames[i];
    }
    return {previousFrame, nextFrame};
}

void Animator::applyPoseToJoints(const Pose& currentPose, Joint& joint, const mat4& parentTransform)
{
    if (currentPose.count(joint.name) == 0)
        return;

    const auto& currentLocalTransform = currentPose.at(joint.name);
    auto currentTransform             = parentTransform * currentLocalTransform;
    joint.animatedTransform           = currentTransform * joint.invtransform;

    for (Joint& childJoint : joint.children)
    {
        applyPoseToJoints(currentPose, childJoint, currentTransform);
    }
}

float Animator::calculateProgression(const KeyFrame& previousFrame, const KeyFrame& nextFrame)
{
    float totalTime   = nextFrame.timeStamp - previousFrame.timeStamp;
    float currentTime = currentTime_ - previousFrame.timeStamp;
    return currentTime / totalTime;
}
}  // namespace Components
}  // namespace GameEngine
