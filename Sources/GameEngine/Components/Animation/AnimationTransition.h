#pragma once
#include "IState.h"
#include "AnimationClipInfo.h"
#include "Context.h"

namespace GameEngine
{
namespace Components
{
struct AnimationTransitionEvent;
struct StateMachine;

class AnimationTransition : public IState
{
public:
    AnimationTransition(Context&, const AnimationClipInfo&, float);
    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

private:
    void calculateTime(float deltaTime);

private:
    Context& context_;
    const AnimationClipInfo info_;

    Animation::KeyFrame startChaneAnimKeyFrame_;
    Animation::KeyFrame endChangeAnimKeyFrame_;

    float timeForChange_;
    float currentTime_;
    float startTime_;
};
}  // namespace Components
}  // namespace GameEngine
