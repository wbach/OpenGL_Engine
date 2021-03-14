#include "IdleState.h"

#include "Attack/AttackEvents.h"

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

void IdleState::onEnter(const AttackEvent &)
{
    DEBUG_LOG("onEnter(const AttackEvent&)");
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void IdleState::onEnter(const EndAttackEvent &)
{
    DEBUG_LOG("onEnter(const EndAttackEvent&)");
    onEnter();
    context_.attackFsm.handle(AttackFsmEvents::End{});
    context_.multiAnimations = false;
}
void IdleState::update(float)
{
}

}  // namespace Components
}  // namespace GameEngine
