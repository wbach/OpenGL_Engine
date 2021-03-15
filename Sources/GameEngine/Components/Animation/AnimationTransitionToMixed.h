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
   // AnimationTransitionToMixed(Context&, const std::vector<CurrentGroupsPlayingInfo>&, const ChangeAnimationEvent&);
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

    struct TransitionGroup
    {
        float startupTime_;
        AnimationClipInfo clipInfo_;

        std::string groupName;
        const std::vector<uint32>& animJointGroup_;

        Animation::KeyFrame startKeyFrame_;

        float timeForChange_;
        float progress_;
    };
    std::vector<TransitionGroup> transtionGroups_;
};
}  // namespace Components
}  // namespace GameEngine
