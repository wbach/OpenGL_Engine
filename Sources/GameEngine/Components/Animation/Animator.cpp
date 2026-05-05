#include "Animator.h"

#include <Common/Transform.h>
#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <algorithm>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "ChangeAnimationEvent.h"
#include "Context.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Animations/Skeleton.h"
#include "GameEngine/Components/Animation/SlaveSkeletonData.h"
#include "GameEngine/Components/Animation/StateMachine.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentType.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "PlayAnimation.h"
#include "StopAnimationEvent.h"
#include "Types.h"

namespace GameEngine
{
using namespace Animation;

namespace Components
{
namespace
{
constexpr char COMPONENT_STR[]                   = "Animator";
constexpr char CSTR_STARTUP_ANIMATION[]          = "startupAnimationClip";
constexpr char CSTR_JOINT_GROUPS[]               = "jointGroups";
constexpr char CSTR_ANIMATION_PLAY_TYPE[]        = "playType";
constexpr char CSTR_ANIMATION_USE_ROOT_MONTION[] = "rootMontion";
constexpr char CSTR_MODEL_BASED_CLIP[]           = "modelBased";
constexpr char CSTR_MONTION_JOINT_NAME[]         = "montionJointName";
constexpr char CSTR_ANIMATION_PLAY_SPEED[]       = "playSpeed";
constexpr char CSTR_DEFAULT_JOINT_GROUP[]        = "deafult";
}  // namespace

Animator::Animator(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<Animator>(), componentContext, gameObject)
    , montionJointName("")
    , masterSkeletonData(componentContext_.graphicsApi_, componentContext_.gpuResourceLoader_)
    , machine_(pose, jointGroupsIds_)
    , montionJoint_{nullptr}
{
}

void Animator::CleanUp()
{
    pose = {};
    machine_.Reset();
    masterSkeletonData.deleteBuffer();
    for (auto& slave : slaveSkeletonData)
    {
        slave.deleteBuffer();
    }
    slaveSkeletonData.clear();
    skeletonDataView.clear();
    animationClipInfo_.clear();
}
void Animator::ReqisterFunctions()
{
    LOG_DEBUG << "ReqisterFunctions";
    RegisterFunction(
        FunctionType::Awake,
        [this]()
        {
            Animator::GetSkeletonAndAnimations();
            auto clipIter = animationClipInfo_.find(startupAnimationClipName);
            if (clipIter != animationClipInfo_.end())
            {
                LOG_DEBUG << "Startup animation found : " << clipIter->first;
                ChangeAnimation(clipIter->first);
            }
            else
            {
                LOG_WARN << "Startup animation not found : " << startupAnimationClipName;
            }
        },
        MakeDependencies<RendererComponent>());
    RegisterFunction(FunctionType::Update, std::bind(&Animator::Update, this));
}

void Animator::Reload()
{
    LOG_DEBUG << "Relaod";

    auto oldAnimationClipInfo = std::move(animationClipInfo_);
    auto oldActiveAnimations  = std::move(activeAnimations_);

    CleanUp();
    GetSkeletonAndAnimations();

    for (auto& [name, info] : oldAnimationClipInfo)
    {
        auto iter = animationClipInfo_.find(name);
        if (iter != animationClipInfo_.end())
        {
            iter->second.subscribers = std::move(info.subscribers);
        }
    }

    if (oldActiveAnimations.size() == 1)
    {
        const auto& name = oldActiveAnimations.begin()->second.clipName;
        LOG_DEBUG << "Restore single animation : " << name;
        SetAnimation(name);
    }
    else if (oldActiveAnimations.size() > 1)
    {
        std::vector<AnimationGroup> animationsToSet;
        for (auto& [group, animation] : oldActiveAnimations)
        {
            LOG_DEBUG << "Restore animations : " << animation.clipName;

            auto clipIter = animationClipInfo_.find(animation.clipName);

            if (clipIter == animationClipInfo_.end())
            {
                LOG_WARN << "Restore animation not found!  : " << animation.clipName;
                return;
            }

            animationsToSet.push_back(AnimationGroup{.groupName = group, .clipInfo = clipIter->second, .time = 0.f});
        }
        machine_.setAnimation(animationsToSet);
    }
    else
    {
        auto clipIter = animationClipInfo_.find(startupAnimationClipName);
        if (clipIter != animationClipInfo_.end())
        {
            LOG_DEBUG << "Startup animation found : " << clipIter->first;
            SetAnimation(clipIter->first);
        }
        else
        {
            LOG_WARN << "Startup animation not found : " << startupAnimationClipName;
        }
    }
}
Animator& Animator::SetAnimation(const std::string& name)
{
    auto clipIter = animationClipInfo_.find(name);
    if (clipIter != animationClipInfo_.end())
    {
        pose.rootMontion = clipIter->second.rootMontion;
        machine_.setAnimation(clipIter->second, 0.f);
        activeAnimations_.clear();
        activeAnimations_[CSTR_DEFAULT_JOINT_GROUP] = ActiveAnimation{.clipName = name};
    }
    return *this;
}
void Animator::StopAnimation(std::optional<std::string> maybeJoingGroupName)
{
    machine_.handle(StopAnimationEvent{maybeJoingGroupName});

    if (maybeJoingGroupName)
    {
        activeAnimations_.erase(*maybeJoingGroupName);
    }
    else
    {
        activeAnimations_.clear();
    }
}
GraphicsApi::ID Animator::getPerPoseBufferId(const RendererComponent& component) const
{
    const auto iter = skeletonDataView.find(&component);
    if (iter != skeletonDataView.end())
    {
        return iter->second->getPerPoseBufferId();
    }
    return std::nullopt;
}
void Animator::setPlayOnceForAnimationClip(const std::string& name)
{
    auto iter = animationClipInfo_.find(name);
    if (iter != animationClipInfo_.end())
    {
        iter->second.playType = AnimationClipInfo::PlayType::once;
    }
}
MaybeId Animator::SubscribeForAnimationFrame(std::string_view animName, std::function<void()> function,
                                             Animation::FrameIndex index)
{
    auto iter = animationClipInfo_.find(animName);
    if (iter != animationClipInfo_.end())
    {
        auto& [_, clipInfo] = *iter;

        const auto& frames = clipInfo.clip.GetFrames();
        if (not frames.empty() and index.value < frames.size())
        {
            auto id              = animationEndIdPool_.getId();
            float frameTimeStamp = frames[static_cast<size_t>(index.value)].timeStamp.value;

            auto& subscribers = clipInfo.subscribers;
            subscribers.push_back({id, function, frameTimeStamp});
            animationClipInfoSubscriptions_.insert({id, &subscribers});

            LOG_DEBUG << "SubscribeForAnimationFrame " << animName << " id : " << id;
            return id;
        }
    }

    LOG_WARN << "SubscribeForAnimationFrame, animation " << animName << " not found or frames are empty!";
    return std::nullopt;
}
MaybeId Animator::SubscribeForAnimationFrame(std::string_view animName, std::function<void()> function, float frameTimeStamp)
{
    auto iter = animationClipInfo_.find(animName);
    if (iter != animationClipInfo_.end() and not iter->second.clip.GetFrames().empty())
    {
        auto& [_, clipInfo] = *iter;
        auto id             = animationEndIdPool_.getId();

        if (frameTimeStamp < 0.0f)
        {
            frameTimeStamp = clipInfo.clip.GetFrames().back().timeStamp.value;
        }

        auto& subscribers = clipInfo.subscribers;
        subscribers.push_back({id, function, frameTimeStamp});
        animationClipInfoSubscriptions_.insert({id, &subscribers});

        LOG_DEBUG << "SubscribeForAnimationFrame " << animName << " id : " << id;
        return id;
    }

    LOG_WARN << "SubscribeForAnimationFrame, animation " << animName << " not found or frames are empty!";
    return std::nullopt;
}
void Animator::UnSubscribeForAnimationFrame(IdType id)
{
    LOG_DEBUG << "Try UnSubscribeForAnimationFrame " << id;
    auto iter = animationClipInfoSubscriptions_.find(id);
    if (iter != animationClipInfoSubscriptions_.end())
    {
        auto& subscribers = *iter->second;
        auto subIter      = std::find_if(subscribers.begin(), subscribers.end(),
                                         [id](const auto& sub)
                                         {
                                        LOG_DEBUG << "UnSubscribeForAnimationFrame " << id;
                                        return sub.id == id;
                                    });

        if (subIter != subscribers.end())
            subscribers.erase(subIter);

        animationClipInfoSubscriptions_.erase(id);
    }
}

Joint* Animator::GetRootJoint()
{
    return &masterSkeletonData.skeleton.getRootJoint();
}
Animation::Joint* Animator::GetJoint(const std::string& name)
{
    return masterSkeletonData.skeleton.getJoint(name);
}

Joint* Animator::GetJoint(const Animation::JointId& id)
{
    return masterSkeletonData.skeleton.getJoint(id);
}

std::optional<uint32> Animator::subscribeForPoseBufferUpdate(std::function<void()> func)
{
    return masterSkeletonData.subscribeForUpdate(func);
}

void Animator::unSubscribeForPoseUpdateBuffer(uint32 id)
{
    masterSkeletonData.unSubscribeForPoseUpdateBuffer(id);
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
        LOG_WARN << "HorizontalAlign not found animation!  : " << animName1 << " or " << animName2;
        return;
    }
    auto& clip1       = clipIter1->second.clip;
    const auto& clip2 = clipIter2->second.clip;

    const auto& clip1Frames = clip1.GetFrames();
    const auto& clip2Frames = clip2.GetFrames();

    if (clip1Frames.empty() or clip2Frames.empty())
    {
        LOG_WARN << "HorizontalAlign empty frames!  : " << animName1 << " or " << animName2;
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
        LOG_WARN << "Last frame not found!";
    }
    LOG_DEBUG << "Aligned animations: " << animName1 << " and " << animName2;
}

bool Animator::isAnimationPlaying(const std::string& name) const
{
    return machine_.currentState_->isAnimationPlaying(name);
}

void Animator::ChangeAnimation(std::string_view name, AnimationChangeType changeType, PlayDirection playDirection,
                               std::optional<std::string> groupName, std::function<void()> onTransitionEnd)
{
    auto clipIter = animationClipInfo_.find(name);

    if (clipIter == animationClipInfo_.end())
    {
        LOG_WARN << "Not found!  : " << name;
        return;
    }

    if (changeType == AnimationChangeType::direct)
    {
        LOG_DEBUG << " AnimationChangeType::direct not implemnted go to smooth";
    }

    LOG_DEBUG << "change " << name;
    handleEvent(ChangeAnimationEvent{0.f, clipIter->second, groupName, onTransitionEnd});
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
    LOG_DEBUG << "";

    rendererComponents_ = thisObject_.GetComponents<RendererComponent>();
    if (rendererComponents_.empty())
    {
        LOG_ERROR << "At least one rendererComponent is required for Animator";
        return;
    }

    masterRendererComponent_ = resolveMasterRendererComponent();

    if (not masterRendererComponent_)
        return;

    initMasterSkeletonData();
    jointsGrupping();
    initSlavesSkeletonsData();
    initAnimationClips();
    Update();
}
void Animator::updateShaderBuffers()
{
    masterSkeletonData.updateBufferTransform();

    for (auto& slave : slaveSkeletonData)
    {
        slave.updateBufferTransform(masterSkeletonData.skeleton);
    }
}
void Animator::Update()
{
    auto status = machine_.update(componentContext_.time_.deltaTime);

    if (status == PoseUpdateAction::update)
    {
        applyPoseToJoints();
    }
}

void Animator::AddAnimationClip(const ReadAnimationInfo& clip)
{
    animationClips.push_back(clip);
}
void Animator::AddAnimationClip(const std::string& name, const GameEngine::File& file, AnimationClipInfo::PlayType playType,
                                AnimationClipInfo::UseRootMontion rootMontion)
{
    animationClips.push_back({name, file, playType == AnimationClipInfo::PlayType::loop, rootMontion});
}
void Animator::AddAnimationClip(const std::string& name, const Animation::AnimationClip& clip,
                                AnimationClipInfo::PlayType playType, AnimationClipInfo::UseRootMontion rootMontion)
{
    auto animationName = name.empty() ? clip.getName() : name;

    if (not animationClipInfo_.count(animationName))
    {
        animationClipInfo_.insert({animationName, AnimationClipInfo{.playSpeed     = 1.f,
                                                                    .playType      = playType,
                                                                    .playDirection = PlayDirection::forward,
                                                                    .rootMontion   = rootMontion,
                                                                    .clip          = clip}});
    }
    else
    {
        LOG_ERROR << "Clip already exist :" << clip.getName();
    }
}
void Animator::applyPoseToJoints(Joint& joint, const mat4& parentTransform)
{
    mat4 parent(parentTransform);
    mat4 currentTransform(parentTransform);

    if (joint.ignoreParentRotation)
    {
        auto invertedParentWithoutTranslation = glm::inverse(parentTransform);
        // remove translation
        invertedParentWithoutTranslation[3][0] = 0;
        invertedParentWithoutTranslation[3][1] = 0;
        invertedParentWithoutTranslation[3][2] = 0;
        // remove rotation from parent
        parent = parent * invertedParentWithoutTranslation;
    }
    auto currentPoseIter = pose.data.find(joint.id);
    if (currentPoseIter != pose.data.end())
    {
        currentTransform = parent * currentPoseIter->second.matrix;
    }
    else
    {
        LOG_DEBUG << "Joint not found ! " << joint;
    }

    joint.worldTransform    = currentTransform * joint.additionalUserMofiyTransform.getMatrix();
    joint.animatedTransform = joint.worldTransform * joint.offset;

    for (Joint& childJoint : joint.children)
    {
        applyPoseToJoints(childJoint, joint.worldTransform);
    }
}
void Animator::applyPoseToJoints()
{
    if (montionJoint_ and pose.rootMontion)
    {
        mat4 meshTransform(1.f);
        if (masterRendererComponent_ and masterRendererComponent_->GetModelWrapper().Get())
        {
            meshTransform = masterRendererComponent_->GetModelWrapper().Get()->GetMeshes()[0].GetMeshTransform();
        }
        else
        {
            LOG_DEBUG << "Renderer component not found";
        }

        const auto& boneSpaceMoveVector = machine_.context_->moveVectorForRootMontion;
        vec3 worldMoveVector            = thisObject_.GetWorldTransform().GetMatrix() *
                               GetRootJoint()->additionalUserMofiyTransform.getMatrix() * meshTransform *
                               vec4(boneSpaceMoveVector.x, 0.f, boneSpaceMoveVector.z, 0.f);
        worldMoveVector.y = 0;

        if (auto rigidbody = thisObject_.GetComponent<Rigidbody>())
        {
            rigidbody->Translate(worldMoveVector);
        }
    }

    applyPoseToJoints(masterSkeletonData.skeleton.getRootJoint(),
                      masterSkeletonData.skeleton.getRotation() * masterSkeletonData.skeleton.getRootJoint().offset);
    updateShaderBuffers();
}
void Animator::initAnimationClips(const Model& model)
{
    for (const auto& [name, clip] : model.animationClips_)
    {
        animationClipInfo_.insert({name, AnimationClipInfo{.playSpeed     = 1.f,
                                                           .playType      = AnimationClipInfo::PlayType::loop,
                                                           .playDirection = PlayDirection::forward,
                                                           .rootMontion   = false,
                                                           .clip          = clip}});
    }
}

void Animator::initAnimationClips()
{
    for (auto& rendererComponent : rendererComponents_)
    {
        if (auto model = rendererComponent->GetModelWrapper().Get(GameEngine::L1))
        {
            initAnimationClips(*model);
        }
    }

    for (auto& clipToRead : animationClips)
    {
        auto playType = clipToRead.playInLoop ? AnimationClipInfo::PlayType::loop : AnimationClipInfo::PlayType::once;
        if (const auto& clip = Animation::ReadAnimationClip(clipToRead.file, masterSkeletonData.skeleton))
        {
            auto animationName = clipToRead.name.empty() ? clip->getName() : clipToRead.name;

            auto iter = animationClipInfo_.find(animationName);
            if (iter != animationClipInfo_.end())
            {
                LOG_DEBUG << "Update anim info for : " << animationName;
                auto& [_, info]  = *iter;
                info.playType    = playType;
                info.rootMontion = clipToRead.useRootMontion;
            }
            else
            {
                LOG_DEBUG << "Add animation file based clip : " << clipToRead.file;
                AddAnimationClip(animationName, *clip, playType, clipToRead.useRootMontion);
            }
            continue;
        }

        auto iter = animationClipInfo_.find(clipToRead.name);
        if (iter != animationClipInfo_.end())
        {
            LOG_DEBUG << "Update anim info for : " << clipToRead.name;
            auto& [_, info]  = *iter;
            info.playType    = playType;
            info.rootMontion = clipToRead.useRootMontion;
        }
    }

    if (animationClipInfo_.size() > 0)
    {
        for (auto* rendererComponent_ : thisObject_.GetComponents<Components::RendererComponent>())
        {
            rendererComponent_->useArmature(true);
        }
    }
}

void Animator::clearAnimationClips()
{
    animationClips.clear();
    animationClipInfo_.clear();
    machine_.Reset();
}

void Animator::printSkeleton(const Animation::Joint& joint, const std::string& hierarchy)
{
    LOG_DEBUG << hierarchy << joint.name;
    const std::string& nextHierarchy{hierarchy + "-"};
    for (const auto& childJoint : joint.children)
    {
        printSkeleton(childJoint, nextHierarchy);
    }
}

ReadAnimationInfo Read(const TreeNode& node)
{
    std::string name{node.getAttributeValue(CSTR_NAME)};
    std::string filename;
    std::string playTypeStr;
    std::string playSpeedStr;
    AnimationClipInfo::UseRootMontion rootMontion{false};

    Read(node.getChild(CSTR_FILE_NAME), filename);
    Read(node.getChild(CSTR_ANIMATION_USE_ROOT_MONTION), rootMontion);
    Read(node.getChild(CSTR_ANIMATION_PLAY_TYPE), playTypeStr);
    Read(node.getChild(CSTR_ANIMATION_PLAY_SPEED), playSpeedStr);

    auto maybePlayType = std::from_string(playTypeStr);
    bool isPlayInLoop{true};
    if (maybePlayType)
    {
        isPlayInLoop = *maybePlayType == AnimationClipInfo::PlayType::loop ? true : false;
    }
    else
    {
        isPlayInLoop = Utils::StringToBool(playTypeStr);
    }

    float playSpeed = 1.f;
    try
    {
        playSpeed = std::stof(playSpeedStr);
    }
    catch (...)
    {
        LOG_ERROR << "playSpeed read error";
    }

    return {name, GameEngine::File(filename), isPlayInLoop, rootMontion, playSpeed};
}

void Animator::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component          = std::make_unique<Animator>(componentContext, gameObject);
        auto animationClipsNode = node.getChild(CSTR_ANIMATION_CLIPS);
        auto montionJointName   = node.getChild(CSTR_MONTION_JOINT_NAME);
        if (montionJointName)
        {
            component->montionJointName = montionJointName->value_;
        }

        if (animationClipsNode)
        {
            for (const auto& childNode : animationClipsNode->getChildren())
            {
                const auto& readInfo = GameEngine::Components::Read(*childNode);
                component->AddAnimationClip(readInfo);
            }
        }

        auto startupAnimationNode = node.getChild(CSTR_STARTUP_ANIMATION);
        if (startupAnimationNode)
        {
            component->startupAnimationClipName = startupAnimationNode->value_;
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

    regsiterComponentReadFunction(GetComponentType<Animator>(), readFunc);
}

void write(TreeNode& node, const ReadAnimationInfo& info)
{
    if (info.file)
    {
        write(node.addChild(CSTR_FILE_NAME), info.file.GetDataRelativePath());
    }
    else
    {
        node.attributes_[CSTR_MODEL_BASED_CLIP] = "true";
    }
    write(node.addChild(CSTR_ANIMATION_PLAY_TYPE),
          std::to_string(info.playInLoop ? AnimationClipInfo::PlayType::loop : AnimationClipInfo::PlayType::once));
    write(node.addChild(CSTR_ANIMATION_USE_ROOT_MONTION), info.useRootMontion);
    write(node.addChild(CSTR_ANIMATION_PLAY_SPEED), info.playSpeed);
}

void Animator::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    node.addChild(CSTR_STARTUP_ANIMATION, startupAnimationClipName);
    node.addChild(CSTR_MONTION_JOINT_NAME, montionJointName);
    auto& animationClipsNode = node.addChild(CSTR_ANIMATION_CLIPS);

    for (const auto& info : animationClips)
    {
        auto& animClipNode               = animationClipsNode.addChild(CSTR_ANIMATION_CLIP);
        animClipNode.attributes_["name"] = info.name;
        ::GameEngine::Components::write(animClipNode, info);
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
RendererComponent* Animator::resolveMasterRendererComponent()
{
    RendererComponent* result{nullptr};
    size_t maxJointCount = 0;

    for (auto& component : rendererComponents_)
    {
        if (not component->IsActive())
        {
            continue;
        }

        if (auto model = component->GetModelWrapper().Get())
        {
            if (const auto& maybeSkeleton = model->getSkeleton())
            {
                if (maxJointCount < maybeSkeleton->getJointsCount())
                {
                    result        = component;
                    maxJointCount = maybeSkeleton->getJointsCount();
                }
            }
        }
    }

    LOG_DEBUG << "resolveMasterRendererComponent with file : "
              << (result ? result->fileName_LOD1.GetFilename() : std::string("-"));
    return result;
}
void Animator::initMasterSkeletonData()
{
    pose = {};
    if (auto model = masterRendererComponent_->GetModelWrapper().Get(GameEngine::L1))
    {
        if (auto maybeSkeleton = model->getSkeleton())
        {
            masterSkeletonData.skeleton = std::move(*maybeSkeleton);
            montionJoint_               = masterSkeletonData.skeleton.getJoint(montionJointName);
            //pose.init(masterSkeletonData.skeleton);

            if (montionJoint_)
            {
                machine_.context_->montionRootJointId = montionJoint_->id;
            }
            else
            {
                LOG_WARN << "Montion joint not found : " << montionJointName;
            }
        }
        else
        {
            LOG_DEBUG << "No root joint " << thisObject_.GetName();
        }

        masterSkeletonData.createBuffer();
        skeletonDataView.insert({masterRendererComponent_, &masterSkeletonData});
    }
    else
    {
        LOG_ERROR << "Model not existing in RendererComponent";
    }
}
void Animator::jointsGrupping()
{
    jointGroupsIds_.clear();
    if (jointGroups_.empty())
    {
        LOG_DEBUG << "create default joint group";
        auto& defaultGroup = jointGroups_[CSTR_DEFAULT_JOINT_GROUP];
        const auto& root   = masterSkeletonData.skeleton.getRootJoint();
        defaultGroup.reserve(root.size);
        createDefaultJointGroup(defaultGroup, masterSkeletonData.skeleton.getRootJoint());
    }

    for (auto& [groupName, jointNamesInGroup] : jointGroups_)
    {
        auto& jointGroupsId = jointGroupsIds_[groupName];
        jointGroupsId.reserve(jointNamesInGroup.size());

        for (auto& name : jointNamesInGroup)
        {
            if (auto joint = masterSkeletonData.skeleton.getJoint(name))
            {
                jointGroupsId.push_back(joint->id);
            }
        }
    }
}
void Animator::initSlavesSkeletonsData()
{
    for (const auto& rendererComponent : rendererComponents_)
    {
        if (rendererComponent == masterRendererComponent_)
            continue;

        auto model = rendererComponent->GetModelWrapper().Get(GameEngine::L1);

        if (model)
        {
            if (auto maybeSkeleton = model->getSkeleton())
            {
                slaveSkeletonData.emplace_back(componentContext_.graphicsApi_, componentContext_.gpuResourceLoader_);
                auto& data    = slaveSkeletonData.back();
                data.skeleton = *maybeSkeleton;
                data.createMapping(masterSkeletonData.skeleton);
                skeletonDataView.insert({rendererComponent, &data});
            }
        }
    }
}
void Animator::handleEvent(const ChangeAnimationEvent& e)
{
    pose.rootMontion = e.info.rootMontion;
    machine_.handle(e);

    if (not e.jointGroupName)
    {
        activeAnimations_.clear();
    }
    activeAnimations_[e.jointGroupName.value_or(CSTR_DEFAULT_JOINT_GROUP)] = ActiveAnimation{.clipName = e.info.clip.getName()};
}
AnimationClipInfo* Animator::getAnimationClipInfo(const std::string& name)
{
    auto iter = animationClipInfo_.find(name);
    if (iter != animationClipInfo_.end())
    {
        return &iter->second;
    }

    return nullptr;
}
}  // namespace Components
}  // namespace GameEngine
