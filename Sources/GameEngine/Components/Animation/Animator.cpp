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
#include "PlayAnimation.h"
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
    auto clipIter = animationClipInfo_.find(name);
    if (clipIter != animationClipInfo_.end())
    {
        machine_.handle(ChangeAnimationEvent{0.f, clipIter->second, std::nullopt});
    }
    return *this;
}
void Animator::StopAnimation(std::optional<std::string> maybeJoingGroupName)
{
    machine_.handle(StopAnimationEvent{maybeJoingGroupName});
}
GraphicsApi::ID Animator::getPerPoseBufferId() const
{
    return jointData_.buffer ? jointData_.buffer->GetGraphicsObjectId() : std::nullopt;
}
void Animator::setPlayOnceForAnimationClip(const std::string& name)
{
    auto iter = animationClipInfo_.find(name);
    if (iter != animationClipInfo_.end())
    {
        iter->second.clip.playType = Animation::AnimationClip::PlayType::once;
    }
}
IdType Animator::SubscribeForAnimationFrame(const std::string& animName, std::function<void()> function, float frame)
{
    auto iter = animationClipInfo_.find(animName);
    if (iter != animationClipInfo_.end() and not iter->second.clip.GetFrames().empty())
    {
        auto id = animationEndIdPool_.getId();

        if (frame < 0.0f)
        {
            frame = iter->second.clip.GetFrames().back().timeStamp;
        }

        auto& subscribers = iter->second.subscribers;
        subscribers.push_back({id, function, frame});
        animationClipInfoSubscriptions_.insert({id, &subscribers});

        DEBUG_LOG("SubscribeForAnimationFrame " + animName + " id : " + std::to_string(id));
        return id;
    }

    WARNING_LOG("SubscribeForAnimationFrame, animation " + animName + " not found or frames are empty!");
    return 0;
}
void Animator::UnSubscribeForAnimationFrame(IdType id)
{
    DEBUG_LOG("Try UnSubscribeForAnimationFrame " + std::to_string(id));
    auto iter = animationClipInfoSubscriptions_.find(id);
    if (iter != animationClipInfoSubscriptions_.end())
    {
        auto& subscribers = *iter->second;
        auto subIter      = std::find_if(subscribers.begin(), subscribers.end(),
                                         [id](const auto& sub)
                                         {
                                        DEBUG_LOG("UnSubscribeForAnimationFrame " + std::to_string(id));
                                        return sub.id == id;
                                    });

        if (subIter != subscribers.end())
            subscribers.erase(subIter);

        animationClipInfoSubscriptions_.erase(id);
    }
}
Animation::Joint* Animator::GetJoint(const std::string& name)
{
    return jointData_.rootJoint.getJoint(name);
}

Joint* Animator::GetJoint(const Animation::JointId& id)
{
    return jointData_.rootJoint.getJoint(id);
}

uint32 Animator::subscribeForPoseBufferUpdate(std::function<void()> func)
{
    if (jointData_.buffer)
        return jointData_.buffer->SubscribeForUpdate(func);

    return 0;
}

void Animator::unSubscribeForPoseUpdateBuffer(uint32 id)
{
    if (jointData_.buffer)
        jointData_.buffer->UnSubscribeForUpdate(id);
}

const Animator::AnimationInfoClips& Animator::getAnimationClips() const
{
    return animationClipInfo_;
}

std::vector<std::string> Animator::getCurrentAnimationName() const
{
    return machine_.currentState_->getCurrentAnimation();
}

void Animator::alignAnimations(const std::string& animName1, const std::string& animName2)
{
    auto clipIter1 = animationClipInfo_.find(animName1);
    auto clipIter2 = animationClipInfo_.find(animName2);

    if (clipIter1 == animationClipInfo_.end() or clipIter2 == animationClipInfo_.end())
    {
        WARNING_LOG("Align not found animation!  : " + animName1 + " or " + animName2);
        return;
    }
    auto& clip1       = clipIter1->second.clip;
    const auto& clip2 = clipIter2->second.clip;

    const auto& clip1Frames = clip1.GetFrames();
    const auto& clip2Frames = clip2.GetFrames();

    if (clip1Frames.empty() or clip2Frames.empty())
    {
        WARNING_LOG("Align empty frames!  : " + animName1 + " or " + animName2);
        return;
    }

    clip1.AddFrame(clip2Frames.front());
    auto lastFrame = clip1.getFrame(clip1Frames[clip1Frames.size() - 1].timeStamp);
    if (lastFrame)
    {
        lastFrame->transforms = clip2Frames.front().transforms;
    }
    else
    {
        WARNING_LOG("Last frame not found!");
    }
    DEBUG_LOG("Aligned animations: " + animName1 + " and " + animName2);
}

void Animator::ChangeAnimation(const std::string& name, AnimationChangeType changeType, PlayDirection playDirection,
                               std::optional<std::string> groupName, std::function<void()> onTransitionEnd)
{
    auto clipIter = animationClipInfo_.find(name);

    if (clipIter == animationClipInfo_.end())
    {
        DEBUG_LOG("Not found!  : " + name);
        return;
    }

    if (changeType == AnimationChangeType::direct)
    {
        DEBUG_LOG(" AnimationChangeType::direct not implemnted go to smooth");
        //        machine_.transitionTo(std::make_unique<PlayAnimation>(machine_.context_, clipIter->second, 0.0f));
        //        if (onTransitionEnd)
        //        {
        //            onTransitionEnd();
        //        }
        //        return;
    }

    machine_.handle(ChangeAnimationEvent{0.f, clipIter->second, groupName, onTransitionEnd});
}
void createDefaultJointGroup(std::vector<std::string>& group, const Animation::Joint& joint)
{
    group.push_back(joint.name);

    for (const auto& child : joint.children)
    {
        createDefaultJointGroup(group, child);
    }
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

            if (jointGroups_.empty())
            {
                DEBUG_LOG("create default joint group");
                createDefaultJointGroup(jointGroups_["deafult"], jointData_.rootJoint);
            }

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
    if (not animationClipInfo_.count(clip.name))
    {
        animationClipInfo_.insert({clip.name, {1.f, clip, PlayPolicy::PlayInLoop, PlayDirection::forward}});
    }
    else
    {
        ERROR_LOG("Clip already exist :" + clip.name);
    }
}

void Animator::applyPoseToJoints(Joint& joint, const mat4& parentTransform)
{
    mat4 parent(parentTransform);
    mat4 currentTransform(parentTransform);

    if (joint.ignoreParentRotation)
    {
        // auto invertedParentWithoutTranslation = glm::mat4(glm::mat3(glm::inverse(parentTransform)));
        auto invertedParentWithoutTranslation = glm::inverse(parentTransform);
        // remove translation
        invertedParentWithoutTranslation[3][0] = 0;
        invertedParentWithoutTranslation[3][1] = 0;
        invertedParentWithoutTranslation[3][2] = 0;
        // remove rotation from parent
        parent = parent * invertedParentWithoutTranslation;
    }
    auto currentPoseIter = jointData_.pose.data.find(joint.id);

    if (currentPoseIter != jointData_.pose.data.end())
    {
        currentTransform = parent * currentPoseIter->second.matrix;
    }
    currentTransform        = currentTransform * joint.additionalUserMofiyTransform;
    joint.animatedTransform = currentTransform * joint.offset;

    for (Joint& childJoint : joint.children)
    {
        applyPoseToJoints(childJoint, currentTransform);
    }
}
void Animator::applyPoseToJoints()
{
    applyPoseToJoints(jointData_.rootJoint, jointData_.rootJoint.offset);
    updateShaderBuffers();
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

    for (const auto& [name, clip] : model.animationClips_)
    {
        auto iter = animationClipInfo_.find(name);
        if (iter == animationClipInfo_.end())
        {
            animationClipInfo_.insert({name, {1.f, clip, PlayPolicy::PlayInLoop, PlayDirection::forward}});
        }
    }

    auto clipIter = animationClipInfo_.find(startupAnimationClipName_);
    if (clipIter != animationClipInfo_.end())
    {
        SetAnimation(clipIter->first);
    }
    else
    {
        WARNING_LOG("Startup animation not found : " + startupAnimationClipName_);
    }

    if (animationClipInfo_.size() > 0)
        rendererComponent_->useArmature(true);
}

void Animator::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
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

    for (const auto& [_, info] : animationClipInfo_)
    {
        if (not info.clip.filePath.empty())
        {
            animationClipsNode.addChild(CSTR_ANIMATION_CLIP, info.clip.filePath);
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
