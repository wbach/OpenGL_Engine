#include "AttackState.h"

#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
AttackState::AttackState(AttackFsmContext &context)
    : context_{context}
{
    if (context_.sendEndEventCallback)
        context_.animator.onAnimationEnd_[context_.attackAnimationName].push_back(context_.sendEndEventCallback);
    else
        WARNING_LOG("sendEndEventCallback not set");
}
void AttackState::onEnter(const AttackFsmEvents::Attack &)
{
    DEBUG_LOG("onEnter(const AttackFsmEvents::Attack&) ");
    if (not context_.attackAnimationName.empty())
    {
        context_.animator.ChangeAnimation(context_.attackAnimationName, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward);
    }
}
void AttackState::onEnter(const AttackFsmEvents::AttackGrouped &event)
{
    if (not context_.attackAnimationName.empty())
    {
        context_.animator.ChangeAnimation(context_.attackAnimationName, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, event.groupName);
    }
}
}  // namespace Components
}  // namespace GameEngine
