#include "IdleStateBase.h"

#include "Attack/AttackEvents.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
IdleStateBase::IdleStateBase(FsmContext &context, const std::string &idleAnimName)
    : StateBase(context)
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
    setIdleAnim();
}

void IdleStateBase::update(float)
{
}

void IdleStateBase::setIdleAnim()
{
    if (not idleAnimName_.empty())
    {
        context_.animator.ChangeAnimation(
            idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void IdleStateBase::update(const WeaponChangeEndEvent &)
{
    context_.multiAnimations        = false;
    context_.weaponChangeTriggered_ = false;
    setIdleAnim();
}

void IdleStateBase::onEnter(const EndForwardMoveEvent &)
{
    setIdleAnim();
}

void IdleStateBase::onEnter(const EndRotationEvent &)
{
    setIdleAnim();
}
}  // namespace Components
}  // namespace GameEngine
