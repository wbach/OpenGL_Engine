#include "Animator.h"

#include <Common/Transform.h>
#include <Logger/Log.h>

#include <algorithm>
#include <string>

#include "ChangeAnimationEvent.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentType.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
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
constexpr char COMPONENT_STR[]                   = "Animator";
constexpr char CSTR_STARTUP_ANIMATION[]          = "startupAnimationClip";
constexpr char CSTR_JOINT_GROUPS[]               = "jointGroups";
constexpr char CSTR_ANIMATION_PLAY_TYPE[]        = "playType";
constexpr char CSTR_ANIMATION_USE_ROOT_MONTION[] = "rootMontion";
constexpr char CSTR_MODEL_BASED_CLIP[]           = "modelBased";
constexpr char CSTR_MONTION_JOINT_NAME[]         = "montionJointName";
constexpr char CSTR_ANIMATION_PLAY_SPEED[]       = "playSpeed";
}  // namespace

Animator::Animator(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<Animator>(), componentContext, gameObject)
    , montionJointName("")
    , jointData_(componentContext_.graphicsApi_)
    , machine_(jointData_.pose, jointGroupsIds_)
    , montionJoint_{nullptr}
{
}

void Animator::CleanUp()
{
    machine_.currentState_.reset();
    if (jointData_.buffer)
    {
        componentContext_.gpuResourceLoader_.AddObjectToRelease(std::move(jointData_.buffer));
    }
}
void Animator::ReqisterFunctions()
{
    LOG_DEBUG << "ReqisterFunctions";
    RegisterFunction(FunctionType::Awake, std::bind(&Animator::GetSkeletonAndAnimations, this),
                     MakeDependencies<RendererComponent>());
    RegisterFunction(FunctionType::Update, std::bind(&Animator::Update, this));
}

Animator& Animator::SetAnimation(const std::string& name)
{
    auto clipIter = animationClipInfo_.find(name);
    if (clipIter != animationClipInfo_.end())
    {
        jointData_.rootMontion = clipIter->second.rootMontion;
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
        iter->second.playType = AnimationClipInfo::PlayType::once;
    }
}

IdType Animator::SubscribeForAnimationFrame(const std::string& animName, std::function<void()> function,
                                            Animation::FrameIndex index)
{
    auto iter = animationClipInfo_.find(animName);
    if (iter != animationClipInfo_.end())
    {
        const auto& frames = iter->second.clip.GetFrames();

        if (not frames.empty() and index.value < frames.size())
        {
            auto id              = animationEndIdPool_.getId();
            float frameTimeStamp = frames[static_cast<size_t>(index.value)].timeStamp.value;

            auto& subscribers = iter->second.subscribers;
            subscribers.push_back({id, function, frameTimeStamp});
            animationClipInfoSubscriptions_.insert({id, &subscribers});

            LOG_DEBUG << "SubscribeForAnimationFrame " << animName << " id : " << id;
            return id;
        }
    }

    LOG_WARN << "SubscribeForAnimationFrame, animation " << animName << " not found or frames are empty!";
    return 0;
}
IdType Animator::SubscribeForAnimationFrame(const std::string& animName, std::function<void()> function, float frameTimeStamp)
{
    auto iter = animationClipInfo_.find(animName);
    if (iter != animationClipInfo_.end() and not iter->second.clip.GetFrames().empty())
    {
        auto id = animationEndIdPool_.getId();

        if (frameTimeStamp < 0.0f)
        {
            frameTimeStamp = iter->second.clip.GetFrames().back().timeStamp.value;
        }

        auto& subscribers = iter->second.subscribers;
        subscribers.push_back({id, function, frameTimeStamp});
        animationClipInfoSubscriptions_.insert({id, &subscribers});

        LOG_DEBUG << "SubscribeForAnimationFrame " << animName << " id : " << id;
        return id;
    }

    LOG_WARN << "SubscribeForAnimationFrame, animation " << animName << " not found or frames are empty!";
    return 0;
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
    return &jointData_.rootJoint;
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
        LOG_WARN << "Align not found animation!  : " << animName1 << " or " << animName2;
        return;
    }
    auto& clip1       = clipIter1->second.clip;
    const auto& clip2 = clipIter2->second.clip;

    const auto& clip1Frames = clip1.GetFrames();
    const auto& clip2Frames = clip2.GetFrames();

    if (clip1Frames.empty() or clip2Frames.empty())
    {
        LOG_WARN << "Align empty frames!  : " << animName1 << " or " << animName2;
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

std::optional<IdType> Animator::allocateIdForClip(const std::string& name)
{
    auto clipIter = animationClipInfo_.find(name);
    if (clipIter == animationClipInfo_.end())
        return std::nullopt;

    auto id = animationClipInfoByIdPool_.getId();
    animationClipInfoById_.insert({id, &clipIter->second});
    return id;
}

void Animator::ChangeAnimation(const std::string& name, AnimationChangeType changeType, PlayDirection playDirection,
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

    jointData_.rootMontion = clipIter->second.rootMontion;
    machine_.handle(ChangeAnimationEvent{0.f, clipIter->second, groupName, onTransitionEnd});
}
void Animator::ChangeAnimation(const IdType& id, AnimationChangeType changeType, PlayDirection playDirection,
                               std::optional<std::string> groupName, std::function<void()> onTransitionEnd)
{
    auto clipIter = animationClipInfoById_.find(id);

    if (clipIter == animationClipInfoById_.end())
    {
        LOG_DEBUG << "ChangeAnimation not found animation with id  : " << id;
        return;
    }

    if (changeType == AnimationChangeType::direct)
    {
        LOG_DEBUG << " AnimationChangeType::direct not implemnted go to smooth";
    }

    jointData_.rootMontion = clipIter->second->rootMontion;
    machine_.handle(ChangeAnimationEvent{0.f, *clipIter->second, groupName, onTransitionEnd});
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

    rendererComponent_ = thisObject_.GetComponent<RendererComponent>();

    if (not rendererComponent_)
    {
        LOG_ERROR << "RendererComponent is required for Animator";
        return;
    }

    auto model = rendererComponent_->GetModelWrapper().Get(GameEngine::L1);

    if (model)
    {
        auto maybeRootJoint = model->getRootJoint();

        LOG_DEBUG << "maybeRootJoint: " << maybeRootJoint;

        if (maybeRootJoint)
        {
            jointData_.rootJoint = *maybeRootJoint;
            initAnimationClips(*model);
            createShaderJointBuffers();

            if (jointGroups_.empty())
            {
                LOG_DEBUG << "create default joint group";
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
            LOG_DEBUG << "Skeleton of: " << model->GetFile().GetBaseName();
            printSkeleton(jointData_.rootJoint);

            montionJoint_ = GetJoint(montionJointName);
            if (montionJoint_)
            {
                LOG_DEBUG << "Montion joint found : " << montionJointName;
                machine_.context_.montionRootJointId = montionJoint_->id;
            }
            else
            {
                LOG_WARN << "Montion joint not found : " << montionJointName;
            }
        }
    }
    else
    {
        LOG_ERROR << "Model not existing in RendererComponent";
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
    currentTransform        = currentTransform * joint.additionalUserMofiyTransform.getMatrix();
    joint.animatedTransform = currentTransform * joint.offset;

    for (Joint& childJoint : joint.children)
    {
        applyPoseToJoints(childJoint, currentTransform);
    }
}
void Animator::applyPoseToJoints()
{
    if (montionJoint_ and jointData_.rootMontion)
    {
        mat4 meshTransform(1.f);
        if (rendererComponent_ && rendererComponent_->GetModelWrapper().Get())
        {
            meshTransform = rendererComponent_->GetModelWrapper().Get()->GetMeshes()[0].GetMeshTransform();
        }
        else
        {
            LOG_DEBUG << "Renderer component not found";
        }

        const auto& boneSpaceMoveVector = machine_.context_.moveVectorForRootMontion;
        vec3 worldMoveVector            = thisObject_.GetWorldTransform().GetMatrix() *
                               GetRootJoint()->additionalUserMofiyTransform.getMatrix() * meshTransform *
                               vec4(boneSpaceMoveVector.x, 0.f, boneSpaceMoveVector.z, 0.f);
        worldMoveVector.y = 0;

        if (auto rigidbody = thisObject_.GetComponent<Rigidbody>())
        {
            rigidbody->Translate(worldMoveVector);
        }
    }

    applyPoseToJoints(jointData_.rootJoint, jointData_.rootJoint.offset);
    updateShaderBuffers();
}
void Animator::createShaderJointBuffers()
{
    if (jointData_.buffer)
    {
        LOG_DEBUG << "ShaderJointBuffer already exist!";
        return;
    }

    jointData_.buffer =
        std::make_unique<BufferObject<PerPoseUpdate>>(componentContext_.graphicsApi_, PER_POSE_UPDATE_BIND_LOCATION);

    LOG_DEBUG << "Created shader buffer: " << thisObject_.GetName() << ", buffer id: " << jointData_.buffer->GetGpuObjectId();

    auto& bufferData = jointData_.buffer->GetData();
    for (size_t i = 0; i < MAX_BONES; ++i)
    {
        bufferData.bonesTransforms[i] = mat4(1.f);
    }
    componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(*jointData_.buffer);
}
void Animator::initAnimationClips(const Model& model)
{
    for (const auto& [name, clip] : model.animationClips_)
    {
        LOG_DEBUG << "Add model based clip : " << name;
        animationClipInfo_.insert({name, AnimationClipInfo{.playSpeed     = 1.f,
                                                           .playType      = AnimationClipInfo::PlayType::loop,
                                                           .playDirection = PlayDirection::forward,
                                                           .rootMontion   = false,
                                                           .clip          = clip}});
    }

    initAnimationClips();
}

void Animator::initAnimationClips()
{
    for (auto& clipToRead : animationClips)
    {
        auto playType = clipToRead.playInLoop ? AnimationClipInfo::PlayType::loop : AnimationClipInfo::PlayType::once;
        if (const auto& clip = Animation::ReadAnimationClip(clipToRead.file, jointData_.rootJoint))
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

    auto clipIter = animationClipInfo_.find(startupAnimationClipName);
    if (clipIter != animationClipInfo_.end())
    {
        SetAnimation(clipIter->first);
    }
    else
    {
        LOG_WARN << "Startup animation not found : " << startupAnimationClipName;
    }

    if (animationClipInfo_.size() > 0)
        rendererComponent_->useArmature(true);
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
}  // namespace Components
}  // namespace GameEngine
