#include "Animator.h"

#include <Common/Transform.h>
#include <Logger/Log.h>

#include <algorithm>

#include "ChangeAnimationEvent.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "StopAnimationEvent.h"

namespace GameEngine
{
using namespace Animation;

namespace Components
{
namespace
{
const std::string COMPONENT_STR{"Animator"};
const std::string CSTR_STARTUP_ANIMATION{"startupAnimationClip"};
const std::string CSTR_JOINT_GROUPS{"jointGroups"};
}  // namespace

Animator::Animator(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(Animator).hash_code(), componentContext, gameObject)
    , jointData_(componentContext_.graphicsApi_)
    , animationSpeed_{1.f}
    , machine_(jointData_.pose, jointGroupsIds_)
{
}

void Animator::CleanUp()
{
    machine_.currentState_.reset();
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
        machine_.handle(ChangeAnimationEvent{
            0.f,
            {animationSpeed_, clipIter->second, PlayPolicy::PlayInLoop, PlayDirection::forward, onAnimationEnd_[name]},
            std::nullopt});
    }
    return *this;
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
IdType Animator::SubscribeForAnimationEnd(const std::string& animName, std::function<void()> function)
{
    auto id = animationEndIdPool_.getId();
    onAnimationEnd_[animName].push_back({id, function});
    return id;
}
void Animator::UnSubscribeForAnimationEnd(IdType id)
{
    for (auto& [_, subscribers] : onAnimationEnd_)
    {
        auto iter =
            std::find_if(subscribers.begin(), subscribers.end(), [id](const auto& pair) { return pair.first == id; });
        if (iter != subscribers.end())
        {
            subscribers.erase(iter);
            return;
        }
    }
}
void Animator::ChangeAnimation(const std::string& name, AnimationChangeType changeType, PlayDirection playDirection,
                               std::optional<std::string> groupName, std::function<void()> onTransitionEnd)
{
    auto clipIter = animationClips_.find(name);

    if (clipIter == animationClips_.end())
    {
        DEBUG_LOG("Not found!  : " + name);
        return;
    }

    machine_.handle(ChangeAnimationEvent{
        0.f,
        {animationSpeed_, clipIter->second, PlayPolicy::PlayInLoop, playDirection, onAnimationEnd_[clipIter->first]},
        groupName,
        onTransitionEnd});
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

            for (auto& [groupName, jointNamesInGroup] : jointGroups_)
            {
                jointGroupsIds_[groupName].reserve(jointNamesInGroup.size());

                for (auto& name : jointNamesInGroup)
                {
                    auto joint = jointData_.rootJoint.getJoint(name);
                    if (joint)
                    {
                        jointGroupsIds_.at(groupName).push_back(joint->id);
                    }
                }
            }
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

        if (not model or model->GetMeshes().empty())
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
    else
    {
        WARNING_LOG("Startup animation not found : " + startupAnimationClipName_);
    }

    if (animationClips_.size() > 0)
        rendererComponent_->useArmature(true);
}
void Animator::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component          = std::make_unique<Animator>(componentContext, gameObject);
        auto animationClipsNode = node.getChild(CSTR_ANIMATION_CLIPS);

        if (animationClipsNode)
        {
            for (const auto& childNode : animationClipsNode->getChildren())
            {
                component->AddAnimationClip(GameEngine::File(childNode->value_));
            }
        }

        auto startupAnimationNode = node.getChild(CSTR_STARTUP_ANIMATION);
        if (startupAnimationNode)
        {
            component->startupAnimationClipName_ = startupAnimationNode->value_;
        }

        auto jointGroupNode = node.getChild(CSTR_JOINT_GROUPS);
        if (jointGroupNode)
        {
            for (auto& node : jointGroupNode->getChildren())
            {
                component->jointGroups_[node->name()] = Utils::SplitString(node->value_, ' ');
            }
        }

        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void Animator::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    node.addChild(CSTR_STARTUP_ANIMATION, startupAnimationClipName_);
    auto& animationClipsNode = node.addChild(CSTR_ANIMATION_CLIPS);

    for (const auto& clip : animationClips_)
    {
        if (not clip.second.filePath.empty())
        {
            animationClipsNode.addChild(CSTR_ANIMATION_CLIP, clip.second.filePath);
        }
    }

    auto& jointGroupNode = node.addChild(CSTR_JOINT_GROUPS);

    for (auto& [groupName, jointNames] : jointGroups_)
    {
        std::string jointNamesValue;
        for (auto& jointName : jointNames)
        {
            jointNamesValue += jointName + " ";
        }

        if (not jointNamesValue.empty())
            jointNamesValue.pop_back();

        jointGroupNode.addChild(groupName, jointNamesValue);
    }
}
}  // namespace Components
}  // namespace GameEngine
