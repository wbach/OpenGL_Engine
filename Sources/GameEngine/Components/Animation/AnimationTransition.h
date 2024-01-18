#pragma once
#include "AnimationClipInfo.h"
#include "Context.h"
#include "AnimationStateBase.h"

namespace GameEngine
{
namespace Components
{
struct AnimationTransitionEvent;
struct StateMachine;

class AnimationTransition : public AnimationStateBase
{
public:
    using OnTransitionEnd = std::function<void()>;

    AnimationTransition(Context&, const AnimationClipInfo&, float, OnTransitionEnd = nullptr);
    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

    std::vector<std::string> getCurrentAnimation() const override;

private:
    void calculateTime(float deltaTime);

private:
    Context& context_;
    const AnimationClipInfo& info_;

    Animation::KeyFrame startChaneAnimKeyFrame_;
    Animation::KeyFrame endChangeAnimKeyFrame_;

    float timeForChange_;
    float currentTime_;
    float startTime_;

    OnTransitionEnd onTransitionEnd_;
};
}  // namespace Components
}  // namespace GameEngine
