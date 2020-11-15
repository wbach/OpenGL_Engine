#include "Animator.h"

#include <Logger/Log.h>

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
        buffer->GetData().bonesTransforms[joint.id] = api_.PrepareMatrixToLoad(joint.animatedTransform);
    }
    for (auto& childJoint : joint.children)
    {
        updateBufferTransform(childJoint);
    }
}

ComponentsType Animator::type = ComponentsType::Animator;

Animator::Animator(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Animator, componentContext, gameObject)
    , jointData_(componentContext_.graphicsApi_)
    , currentTime_(0.f)
    , animationSpeed_(1.f)
    , changeAnimTime_(0.25f)
    , current_("DefaultAnimationClip")
    , changeAnim(false)
{
}

void Animator::CleanUp()
{
    if (jointData_.buffer and jointData_.buffer->GetGraphicsObjectId())
        componentContext_.gpuResourceLoader_.AddObjectToRelease(std::move(jointData_.buffer));
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
GraphicsApi::ID Animator::getPerPoseBufferId() const
{
    return jointData_.buffer ? jointData_.buffer->GetGraphicsObjectId() : std::nullopt;
}
void Animator::ChangeAnimation(const std::string& name)
{
    if (not animationClips_.count(name))
        return;

    auto& endFrames = animationClips_.at(name).GetFrames();
    if (endFrames.empty())
        return;

    endChangeAnimPose = endFrames[0];

    nextClip_              = name;
    changeAnim             = true;
    currentChangeAnimTime_ = 0;

    startChaneAnimPose = KeyFrame();
    for (const auto& p : calculateCurrentAnimationPose())
        startChaneAnimPose.transforms.insert({p.first, GetJointTransform(p.second)});
}
void Animator::GetSkeletonAndAnimations()
{
    auto renderer = thisObject_.GetComponent<RendererComponent>();

    if (renderer == nullptr)
        return;

    auto model = renderer->GetModelWrapper().Get(GameEngine::L1);

    if (model)
    {
        auto maybeRootJoint = model->getRootJoint();

        if (maybeRootJoint)
        {
            jointData_.rootJoint = *maybeRootJoint;
            jointData_.buffer    = std::make_unique<BufferObject<PerPoseUpdate>>(componentContext_.graphicsApi_,
                                                                              PER_POSE_UPDATE_BIND_LOCATION);

            auto& bufferData = jointData_.buffer->GetData();
            for (size_t i = 0; i < MAX_BONES; ++i)
            {
                bufferData.bonesTransforms[i] = mat4(1.f);
            }
            componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(*jointData_.buffer);
        }

        for (const auto& clip : model->animationClips_)
        {
            animationClips_.insert(clip);
        }

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
    jointData_.updateBufferTransform();
    componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(*jointData_.buffer);
}
void Animator::ChangeAnimState()
{
    currentChangeAnimTime_ += (1.f / changeAnimTime_) * componentContext_.time_.deltaTime * fabsf(animationSpeed_);

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
    applyPoseToJoints(pose, jointData_.rootJoint, mat4(1.f));

    updateShaderBuffers();
}
bool Animator::IsReady()
{
    return (not current_.empty() and animationClips_.count(current_));
}
void Animator::Update()
{
    if (not IsReady())
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
void Animator::AddAnimationClip(const GameEngine::File& file)
{
    AddAnimationClip(Animation::ReadAnimationClip(file));
}
void Animator::AddAnimationClip(const Animation::AnimationClip& clip)
{
    if (not animationClips_.count(clip.name))
    {
        animationClips_.insert({clip.name, clip});
    }
    else
    {
        ERROR_LOG("Clip already exist :" + clip.name);
    }
}
void Animator::increaseAnimationTime()
{
    auto animationLength = animationClips_.at(current_).GetLength();

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
        const auto& jointName = pair.first;
        const auto& nextFrameTransformIter = nextFrame.transforms.find(jointName);

        if (nextFrameTransformIter != nextFrame.transforms.cend())
        {
            const auto& previousTransform = pair.second;
            const auto& nextTransform = nextFrameTransformIter->second;
            JointTransform currentTransform = Interpolate(previousTransform, nextTransform, progression);
            currentPose.insert({ jointName, GetLocalTransform(currentTransform) });
        }
    }
    return currentPose;
}

std::pair<KeyFrame, KeyFrame> Animator::getPreviousAndNextFrames()
{
    const auto& allFrames = animationClips_.at(current_).GetFrames();

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
    mat4 currentTransform(1.f);

    auto currentPoseIter = currentPose.find(joint.name);

    if (currentPoseIter != currentPose.end())
    {
        const auto& currentLocalTransform = currentPoseIter->second;
        currentTransform                  = parentTransform * currentLocalTransform;
        joint.animatedTransform           = currentTransform * joint.offset;
    }

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
