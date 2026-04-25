#include "IdleArmedChangeState.h"

#include <Logger/Log.h>

#include "../CharacterController.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
IdleArmedChangeState::IdleArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, std::nullopt)
    , IdleStateBase{context, context.animClipNames.disarmed.posture.stand.idle}
    , context_{context}
{
}

void IdleArmedChangeState::onEnter(const CrouchChangeStateEvent &)
{
}

void IdleArmedChangeState::onEnter(IdleCrouchArmedChangeState &, const CrouchChangeStateEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void IdleArmedChangeState::onEnter(DisarmedAttackState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
}
void IdleArmedChangeState::onEnter(DisarmedIdleState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
}
void IdleArmedChangeState::onEnter(DisarmedAttackState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
}
void IdleArmedChangeState::onEnter(ArmedIdleState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}
void IdleArmedChangeState::onEnter(ArmedAttackState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}
void IdleArmedChangeState::onEnter(DisarmedIdleState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
}
void IdleArmedChangeState::onEnter(DrawArrowState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}
void IdleArmedChangeState::onEnter(RecoilState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}
void IdleArmedChangeState::onEnter(AimState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}
void IdleArmedChangeState::onEnter(const EndForwardMoveEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void IdleArmedChangeState::onEnter(const EndBackwardMoveEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void IdleArmedChangeState::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void IdleArmedChangeState::update(float)
{
}
bool IdleArmedChangeState::entryCondition(DisarmedIdleState &) const
{
    return not context_.animClipNames.equip.empty() and hasWeapon();
}
bool IdleArmedChangeState::entryCondition(DisarmedAttackState &) const
{
    return not context_.animClipNames.equip.empty() and hasWeapon();
}
bool IdleArmedChangeState::entryCondition(ArmedIdleState &) const
{
    return not context_.animClipNames.disarm.empty() and hasWeapon();
}
bool IdleArmedChangeState::entryCondition(ArmedAttackState &) const
{
    return not context_.animClipNames.disarm.empty() and hasWeapon();
}
}  // namespace Components
}  // namespace GameEngine
