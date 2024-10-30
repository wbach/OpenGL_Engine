#pragma once
#include "AnimationClipInfo.h"
#include "AnimationStateBase.h"
#include "Context.h"

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
    Context& context;
    TransitionGroup data;
};
}  // namespace Components
}  // namespace GameEngine
