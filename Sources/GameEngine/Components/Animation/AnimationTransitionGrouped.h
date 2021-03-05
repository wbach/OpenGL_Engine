#pragma once
#include "AnimationClipInfo.h"
#include "Context.h"
#include "IState.h"

namespace GameEngine
{
namespace Components
{
class ChangeAnimationEvent;

class AnimationTransitionGrouped : public IState
{
public:
    AnimationTransitionGrouped(Context&, const AnimationClipInfo&, float, const ChangeAnimationEvent&);
    bool update(float) override;
    const std::string &getAnimationClipName() const override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

private:
    void increaseAnimationTime(float);
    void increaseTransitionTime(float);

private:
    Context& context_;
    AnimationClipInfo currentClipInfo_;
    float currentClipProgres_;
    std::vector<uint32> currentAnimJointGroup_;

    float secondaryClipStartupTime_;
    AnimationClipInfo secondaryClipInfo_;
    std::vector<uint32> secondaryAnimJointGroup_;

    Animation::KeyFrame startChaneAnimKeyFrame_;
    Animation::KeyFrame endChangeAnimKeyFrame_;

    float startTime_;
    float timeForChange_;
    float transitionProgress_;

};
}  // namespace Components
}  // namespace GameEngine
