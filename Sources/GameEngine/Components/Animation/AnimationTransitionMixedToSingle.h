#pragma once
#include "AnimationClipInfo.h"
#include "Context.h"
#include "CurrentGroupsPlayingInfo.h"
#include "IState.h"

namespace GameEngine
{
namespace Components
{
class ChangeAnimationEvent;

class AnimationTransitionMixedToSingle : public IState
{
public:
    AnimationTransitionMixedToSingle(Context&, float, const AnimationClipInfo&, const std::vector<std::string>&);
    AnimationTransitionMixedToSingle(Context&, const std::vector<CurrentGroupsPlayingInfo>&, const ChangeAnimationEvent&);
    bool update(float) override;
    const std::string& getAnimationClipName() const override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

private:
    void increaseAnimationTime(float);
    void increaseTransitionTime(float);

private:
    Context& context_;
    struct Group
    {
        AnimationClipInfo currentClipInfo_;
        float currentClipProgres_;
        std::vector<uint32> currentAnimJointGroup_;
    };
    std::vector<Group> currentGroups_;

    float secondaryClipStartupTime_;
    AnimationClipInfo secondaryClipInfo_;

    std::string secondaryJointGroupName;
    std::vector<uint32> secondaryAnimJointGroup_;

    Animation::KeyFrame startChaneAnimKeyFrame_;

    float startTime_;
    float timeForChange_;
    float transitionProgress_;
};
}  // namespace Components
}  // namespace GameEngine
