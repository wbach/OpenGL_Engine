#include "IdleStateBase.h"

#include "Attack/AttackEvents.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
IdleStateBase::IdleStateBase(FsmContext &context, const std::string &idleAnimName)
    : StateBase(context)
    , context_{context}
    , idleAnimName_{idleAnimName}
{
}

void IdleStateBase::update(const AttackEvent &)
{
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void IdleStateBase::update(const EndAttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::End{});
    context_.multiAnimations = false;
    onEnter();
}

void IdleStateBase::update(float)
{
}

void IdleStateBase::onLeave()
{
}

void IdleStateBase::enter()
{
    if (not idleAnimName_.empty())
    {
        context_.animator.ChangeAnimation(
            idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void IdleStateBase::onWeaponChanged()
{
    if (not idleAnimName_.empty())
    {
        context_.animator.ChangeAnimation(
            idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

}  // namespace Components
}  // namespace GameEngine
