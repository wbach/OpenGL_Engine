#pragma once
#include "AnimationClipInfo.h"
#include "Context.h"
#include "CurrentGroupsPlayingInfo.h"
#include "IState.h"

namespace GameEngine
{
namespace Components
{
struct ChangeAnimationEvent;

class AnimationTransitionMixedToSingle : public IState
{
public:
    AnimationTransitionMixedToSingle(Context&, const CurrentGroupsPlayingInfo&);
    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

private:
    void increaseAnimationTime(float);
    void increaseTransitionTime(float);

private:
    Context& context_;

    float currentClipProgres_;
    const AnimationClipInfo& currentClipInfo_;

    struct Group
    {
        std::string name;
        std::vector<uint32> currentAnimJointGroup_;
    };
    std::vector<Group> currentGroups_;

    Animation::KeyFrame startChaneAnimKeyFrame_;

    struct TransitionGroups
    {
        const std::vector<uint32>& jointGroups;
    };
    std::vector<TransitionGroups> transitionGroups_;

    float transitionProgress_;
};
}  // namespace Components
}  // namespace GameEngine
