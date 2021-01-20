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
        machine_.handle(std::make_unique<PlayAnimationEvent>(jointData_.pose, clipIter->second, animationSpeed_,
                                                             PlayDirection::forward, onAnimationEnd_[name]));
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
std::optional<uint32> Animator::connectBoneWithObject(const std::string& jointName, GameObject& object,
                                                      const std::optional<vec3>& po, const std::optional<Rotation>& ro)
{
    auto bone = jointData_.rootJoint.getJoint(jointName);
    if (bone)
    {
        connectedObjects_.insert({bone->id, {object, po ? *po : vec3(0), ro ? *ro : Rotation(RadiansVec3(0))}});
        return bone->id;
    }
    ERROR_LOG("Joint not found. Name :" + jointName);
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
        machine_.handle(std::make_unique<PlayAnimationEvent>(jointData_.pose, info));
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
            initAnimationClips(*model);
            createShaderJointBuffers();
        }
    }
}
void Animator::updateShaderBuffers()
{
    jointData_.updateBufferTransform();
    if (jointData_.buffer)
    {
        componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(*jointData_.buffer);
    }
}
void Animator::Update()
{
    machine_.processEvents();
    auto status = machine_.update(componentContext_.time_.deltaTime);

    if (status == PoseUpdateAction::update)
    {
        applyPoseToJoints();
    }
}
void Animator::AddAnimationClip(const GameEngine::File& file)
{
    clipsToRead_.push_back(file);
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

void Animator::applyPoseToJoints(Joint& joint, const mat4& parentTransform)
{
    mat4 currentTransform(1.f);

    auto currentPoseIter = jointData_.pose.find(joint.id);

    if (currentPoseIter != jointData_.pose.end())
    {
        const auto& currentLocalTransform = currentPoseIter->second.matrix;
        currentTransform                  = parentTransform * currentLocalTransform;

        joint.animatedTransform = currentTransform * joint.offset;
        updateConnectedObjectToJoint(joint.id, joint);
    }

    for (Joint& childJoint : joint.children)
    {
        applyPoseToJoints(childJoint, currentTransform);
    }
}
void Animator::applyPoseToJoints()
{
    applyPoseToJoints(jointData_.rootJoint, mat4(1.f));
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
void Animator::createShaderJointBuffers()
{
    jointData_.buffer =
        std::make_unique<BufferObject<PerPoseUpdate>>(componentContext_.graphicsApi_, PER_POSE_UPDATE_BIND_LOCATION);

    auto& bufferData = jointData_.buffer->GetData();
    for (size_t i = 0; i < MAX_BONES; ++i)
    {
        bufferData.bonesTransforms[i] = mat4(1.f);
    }
    componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(*jointData_.buffer);
}
void Animator::initAnimationClips(const Model& model)
{
    for (auto& file : clipsToRead_)
    {
        AddAnimationClip(Animation::ReadAnimationClip(file, jointData_.rootJoint));
    }

    for (const auto& clip : model.animationClips_)
    {
        animationClips_.insert(clip);
    }

    auto clipIter = animationClips_.find(startupAnimationClipName_);
    if (clipIter != animationClips_.end())
    {
        SetAnimation(clipIter->first);
    }

    if (animationClips_.size() > 0)
        rendererComponent_->useArmature(true);
}
}  // namespace Components
}  // namespace GameEngine
