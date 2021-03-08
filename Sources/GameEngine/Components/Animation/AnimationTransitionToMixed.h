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

class AnimationTransitionToMixed : public IState
{
public:
    AnimationTransitionToMixed(Context&, const std::vector<CurrentGroupsPlayingInfo>&, const ChangeAnimationEvent&);
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
        AnimationClipInfo clipInfo_;
        float progres_;
        const std::vector<uint32>& jointGroup_;
        std::string name;
    };
    std::vector<Group> currentGroups_;

    float secondaryClipStartupTime_;
    AnimationClipInfo secondaryClipInfo_;

    std::string secondaryJointGroupName;
    std::string secondaryGroupName;
    const std::vector<uint32>* secondaryAnimJointGroup_;

    Animation::KeyFrame startChaneAnimKeyFrame_;

    float timeForChange_;
    float transitionProgress_;
};
}  // namespace Components
}  // namespace GameEngine
