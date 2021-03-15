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
        context_.animator.ChangeAnimation(
            context_.idleAnimationName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void IdleState::update(const AttackEvent &)
{
    DEBUG_LOG("update(const AttackEvent&)");
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void IdleState::update(const EndAttackEvent &)
{
    DEBUG_LOG("update(const EndAttackEvent&)");
    context_.attackFsm.handle(AttackFsmEvents::End{});
    context_.multiAnimations = false;
    onEnter();
}
void IdleState::update(float)
{
}

}  // namespace Components
}  // namespace GameEngine
