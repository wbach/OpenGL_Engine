#include "Animator.h"

#include <Common/Transform.h>
#include <Logger/Log.h>

#include <algorithm>

#include "AnimationTransitionEvent.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "PlayAnimationEvent.h"

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
    , animationSpeed_{1.f}
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
    auto clipIter = animationClips_.find(name);
    if (clipIter != animationClips_.end())
    {
        machine_.handle(std::make_unique<PlayAnimationEvent>(clipIter->second, animationSpeed_, PlayDirection::forward,
                                                             onAnimationEnd_[name]));
    }
    else
    {
        requestedAnimationToset_ = name;
    }
    return *this;
}
const std::string& Animator::GetCurrentAnimationName() const
{
    return machine_.getCurrentAnimationClipName();
}
GraphicsApi::ID Animator::getPerPoseBufferId() const
{
    return jointData_.buffer ? jointData_.buffer->GetGraphicsObjectId() : std::nullopt;
}
std::optional<uint32> Animator::connectBoneWithObject(const std::string& boneName, GameObject& object,
                                                      const std::optional<vec3>& po, const std::optional<Rotation>& ro)
{
    auto bone = jointData_.rootJoint.getChild(boneName);
    if (bone)
    {
        connectedObjects_.insert({bone->id, {object, po ? *po : vec3(0), ro ? *ro : Rotation(RadiansVec3(0))}});
        return bone->id;
    }
    ERROR_LOG("Bone not found. Name :" + boneName);
    return std::nullopt;
}
void Animator::disconnectObjectFromBone(uint32 boneId)
{
    connectedObjects_.erase(boneId);
}
void Animator::setPlayOnceForAnimationClip(const std::string& name)
{
    auto iter = animationClips_.find(name);
    if (iter != animationClips_.end())
    {
        iter->second.playType = Animation::AnimationClip::PlayType::once;
    }
}
void Animator::ChangeAnimation(const std::string& name, AnimationChangeType changeType, PlayDirection playDirection)
{
    auto clipIter = animationClips_.find(name);

    if (clipIter == animationClips_.end())
    {
        return;
    }
    AnimationPlayingInfo info{clipIter->second, animationSpeed_, playDirection, onAnimationEnd_[clipIter->first]};

    if (changeType == AnimationChangeType::direct)
    {
        machine_.handle(std::make_unique<PlayAnimationEvent>(info));
        return;
    }

    machine_.handle(std::make_unique<AnimationTransitionEvent>(jointData_.pose, info));
}
void Animator::GetSkeletonAndAnimations()
{
    rendererComponent_ = thisObject_.GetComponent<RendererComponent>();

    if (not rendererComponent_)
        return;

    auto model = rendererComponent_->GetModelWrapper().Get(GameEngine::L1);

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

            if (requestedAnimationToset_ == clip.first)
            {
                SetAnimation(clip.first);
                requestedAnimationToset_.clear();
            }
        }

        if (animationClips_.size() > 0)
            rendererComponent_->useArmature(true);
    }
}
void Animator::updateShaderBuffers()
{
    jointData_.updateBufferTransform();
    componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(*jointData_.buffer);
}
bool Animator::IsReady()
{
    // return (currentAnimationClip_ != nullptr and not currentAnimationClip_->GetFrames().empty());
    return true;
}
void Animator::Update()
{
    if (not IsReady())
        return;

    machine_.processEvents();
    auto maybePose = machine_.update(componentContext_.time_.deltaTime);

    if (maybePose)
    {
        applyPoseToJoints(*maybePose);
    }
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

void Animator::applyPoseToJoints(const Pose& currentPose, Joint& joint, const mat4& parentTransform)
{
    mat4 currentTransform(1.f);

    auto currentPoseIter = currentPose.find(joint.name);

    if (currentPoseIter != currentPose.end())
    {
        const auto& currentLocalTransform = currentPoseIter->second;
        currentTransform                  = parentTransform * currentLocalTransform;

        joint.animatedTransform = currentTransform * joint.offset;
        updateConnectedObjectToJoint(joint.id, joint);
    }

    for (Joint& childJoint : joint.children)
    {
        applyPoseToJoints(currentPose, childJoint, currentTransform);
    }
}
void Animator::applyPoseToJoints(const Pose& pose)
{
    jointData_.pose = pose;
    applyPoseToJoints(pose, jointData_.rootJoint, mat4(1.f));
    updateShaderBuffers();
}
void Animator::updateConnectedObjectToJoint(uint32 jointId, const Animation::Joint& joint)
{
    auto connectedObjectIter = connectedObjects_.find(jointId);
    if (connectedObjectIter != connectedObjects_.end())
    {
        auto rendererCopmponent = thisObject_.GetComponent<RendererComponent>();

        if (not rendererCopmponent)
            return;

        auto model = rendererCopmponent->GetModelWrapper().Get();

        if (not model or model->GetMeshes().size() < 1)
            return;

        const auto& meshTransfrom = model->GetMeshes().front().GetMeshTransform();

        // path bone to world pos
        // worldPosition = parentMatrix * parentMeshMatrix * jointMatrix * inverse(jointOffset) * point

        auto worldBoneMatrix =
            thisObject_.GetWorldTransform().GetMatrix() * meshTransfrom * joint.animatedTransform * joint.offset;

        worldBoneMatrix = worldBoneMatrix * glm::translate(connectedObjectIter->second.worldPositionOffset);

        auto [boneWorldPosition, boneWorldRotation, _] = Utils::decompose(worldBoneMatrix);

        const auto& rotationOffset = connectedObjectIter->second.worldRotationOffset;
        connectedObjectIter->second.gameObject.SetWorldPositionRotation(boneWorldPosition,
                                                                        boneWorldRotation * rotationOffset.value_);
    }
}
}  // namespace Components
}  // namespace GameEngine
