#pragma once
#include "AnimationClipInfo.h"
#include "AnimationStateBase.h"
#include "Context.h"
#include "CurrentGroupsPlayingInfo.h"
#include "TransitionGroupsPlaying.h"

namespace GameEngine
{
namespace Components
{
struct ChangeAnimationEvent;

class AnimationTransitionToMixed : public AnimationStateBase
{
public:
    AnimationTransitionToMixed(Context&, const std::vector<CurrentGroupsPlayingInfo>&, const ChangeAnimationEvent&,
                               std::function<void()> = nullptr);
    AnimationTransitionToMixed(Context&, const std::vector<TransitionGroupsPlaying>&, const ChangeAnimationEvent&,
                               std::function<void()> = nullptr);
    AnimationTransitionToMixed(Context&, const std::vector<CurrentGroupsPlayingInfo>&,
                               const std::vector<TransitionGroupsPlaying>&, const ChangeAnimationEvent&,
                               std::function<void()> = nullptr);
    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

    std::vector<std::string> getCurrentAnimation() const override;
    bool isAnimationPlaying(const std::string&) const override;

private:
    void increaseAnimationTime(float);
    void increaseTransitionTime(float);
    void addTransitionBasedOnEvent(const ChangeAnimationEvent&);

private:
    Context& context_;

    struct Group
    {
        const AnimationClipInfo& clipInfo_;
        float time_;
        const std::vector<uint32>& jointGroup_;
        CurrentFrames frames{nullptr, nullptr};
        float previousFrameTimeStamp{-1.0f};
    };
    std::unordered_map<std::string, Group> currentGroups_;

    struct TransitionGroup
    {
        float startupTime_;
        const AnimationClipInfo& clipInfo_;
        const std::vector<uint32>& jointGroup_;
        std::function<void()> onTransitionEnd_;
        Animation::KeyFrame startKeyFrame_;
        float timeForChange_;
        float progress_;
    };
    std::unordered_map<std::string, TransitionGroup> transtionGroups_;
};
}  // namespace Components
}  // namespace GameEngine
