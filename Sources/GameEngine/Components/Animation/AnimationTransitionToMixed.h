#pragma once
#include "AnimationStateBase.h"

namespace GameEngine
{
namespace Components
{
struct ChangeAnimationEvent;
struct CurrentGroupsPlayingInfo;
struct TransitionGroupsPlaying;

class AnimationTransitionToMixed : public AnimationStateBase
{
public:
    using CurrentGroupsPlayingInfos = std::vector<CurrentGroupsPlayingInfo>;
    using TransitionGroupsPlayings  = std::vector<TransitionGroupsPlaying>;

    AnimationTransitionToMixed(Context&, const CurrentGroupsPlayingInfos&, const ChangeAnimationEvent&);
    AnimationTransitionToMixed(Context&, const TransitionGroupsPlayings&, const ChangeAnimationEvent&);
    AnimationTransitionToMixed(Context&, const CurrentGroupsPlayingInfos&, const TransitionGroupsPlayings&,
                               const ChangeAnimationEvent&);

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
    std::unordered_map<std::string, PlayGroupMixed> currentGroups;
    std::unordered_map<std::string, TransitionGroup> transtionGroups;
};
}  // namespace Components
}  // namespace GameEngine
