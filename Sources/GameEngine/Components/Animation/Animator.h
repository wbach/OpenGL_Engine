#pragma once
#include <GameEngine/Components/Animation/AnimationClipInfo.h>

#include <unordered_map>

#include "Common.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/ReadAnimationInfo.h"
#include "JointData.h"
#include "PlayDirection.h"
#include "StateMachine.h"

namespace common
{
class Trasnform;
}  // namespace common

namespace GameEngine
{
class Mesh;
class Model;
class File;

namespace Components
{
class RendererComponent;

class Animator : public BaseComponent
{
public:
    std::string montionJointName;
    std::string startupAnimationClipName;
    std::vector<ReadAnimationInfo> animationClips;
    JointGroups jointGroups_;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_STRING(montionJointName)
        FIELD_STRING(startupAnimationClipName)
        FIELD_VECTOR_OF_ANIMATION_CLIPS(animationClips)
        // jointGroups_
    END_FIELDS()
    // clang-format on
public:
    using AnimationInfoClips      = std::unordered_map<std::string, AnimationClipInfo>;
    using AnimationInfoClipsIdMap = std::unordered_map<IdType, AnimationClipInfo*>;

    enum class AnimationChangeType
    {
        smooth,
        direct
    };

    Animator(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void Update();
    void AddAnimationClip(const ReadAnimationInfo&);
    void AddAnimationClip(const std::string&, const GameEngine::File&,
                          AnimationClipInfo::PlayType       = AnimationClipInfo::PlayType::loop,
                          AnimationClipInfo::UseRootMontion = false);
    void AddAnimationClip(const std::string&, const Animation::AnimationClip&,
                          AnimationClipInfo::PlayType       = AnimationClipInfo::PlayType::loop,
                          AnimationClipInfo::UseRootMontion = false);
    Animator& SetAnimation(const std::string&);
    void ChangeAnimation(const std::string&, AnimationChangeType = AnimationChangeType::smooth,
                         PlayDirection = PlayDirection::forward, std::optional<std::string> = std::nullopt,
                         std::function<void()> = nullptr);
    void ChangeAnimation(const IdType&, AnimationChangeType = AnimationChangeType::smooth, PlayDirection = PlayDirection::forward,
                         std::optional<std::string> = std::nullopt, std::function<void()> = nullptr);

    void StopAnimation(std::optional<std::string> = std::nullopt);
    GraphicsApi::ID getPerPoseBufferId() const;
    void setPlayOnceForAnimationClip(const std::string&);

    IdType SubscribeForAnimationFrame(const std::string&, std::function<void()>, Animation::FrameIndex);
    IdType SubscribeForAnimationFrame(const std::string&, std::function<void()>, float = -1);
    void UnSubscribeForAnimationFrame(IdType);

    Animation::Joint* GetRootJoint();
    Animation::Joint* GetJoint(const std::string&);
    Animation::Joint* GetJoint(const Animation::JointId&);

    uint32 subscribeForPoseBufferUpdate(std::function<void()>);
    void unSubscribeForPoseUpdateBuffer(uint32);

    const AnimationInfoClips& getAnimationClips() const;
    std::vector<std::string> getCurrentAnimationName() const;

    void alignAnimations(const std::string&, const std::string&);

    bool isAnimationPlaying(const std::string&) const;

    std::optional<IdType> allocateIdForClip(const std::string&);
    void initAnimationClips();
    void clearAnimationClips();

public:
    JointData jointData_;
    float animationSpeed_;

protected:
    void updateShaderBuffers();
    void GetSkeletonAndAnimations();
    void applyPoseToJoints(Animation::Joint&, const mat4&);
    void applyPoseToJoints();
    void createShaderJointBuffers();
    void initAnimationClips(const Model&);
    void printSkeleton(const Animation::Joint&, const std::string& = "");

protected:
    StateMachine machine_;

    AnimationInfoClips animationClipInfo_;
    AnimationInfoClipsIdMap animationClipInfoById_;

    std::unordered_map<IdType, std::vector<AnimationClipInfo::Subscription>*> animationClipInfoSubscriptions_;

    Utils::IdPool animationEndIdPool_;
    Utils::IdPool animationClipInfoByIdPool_;

    RendererComponent* rendererComponent_;
    JointGroupsIds jointGroupsIds_;

    Animation::Joint* montionJoint_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
