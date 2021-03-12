#include "IdleState.h"

namespace GameEngine
{
namespace Components
{
IdleState::IdleState(FsmContext &context)
    : context_{context}
{
}
void IdleState::onEnter()
{
    DEBUG_LOG("onEnter()");
    if (not context_.idleAnimationName.empty())
    {
        context_.animator.ChangeAnimation(context_.idleAnimationName, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, std::nullopt);
    }
}
void IdleState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
