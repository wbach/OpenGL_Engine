#include "IdleCrouchArmedChangeState.h"

#include <Logger/Log.h>

#include "../CharacterController.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
IdleCrouchArmedChangeState::IdleCrouchArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , IdleStateBase{context, context.animClipNames.armed.posture.crouched.idle}
    , context_{context}
{
}

void IdleCrouchArmedChangeState::onEnter(const CrouchChangeStateEvent &)
{
}

void IdleCrouchArmedChangeState::onEnter(IdleArmedChangeState &, const CrouchChangeStateEvent &)
{
    idleAnimName_ = context_.animClipNames.armed.posture.crouched.idle;
    setAnim();
}
void IdleCrouchArmedChangeState::onEnter(DisarmedIdleCrouchState &, const WeaponStateEvent &)
{
    idleAnimName_ = context_.animClipNames.armed.posture.crouched.idle;
    setAnim();
    ArmedChangeStateBase::equipWeapon();
}
void IdleCrouchArmedChangeState::onEnter(ArmedIdleCrouchState &, const WeaponStateEvent &)
{
    idleAnimName_ = context_.animClipNames.disarmed.posture.crouched.idle;
    setAnim();
    ArmedChangeStateBase::disarmWeapon();
}

void IdleCrouchArmedChangeState::onEnter(DisarmedIdleCrouchState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
}

void IdleCrouchArmedChangeState::onEnter(AimCrouchState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}

void IdleCrouchArmedChangeState::onEnter(DrawArrowCrouchState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}

void IdleCrouchArmedChangeState::onEnter(RecoilCrouchState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}

void IdleCrouchArmedChangeState::update(float)
{
}

void IdleCrouchArmedChangeState::setAnim()
{
    if (not idleAnimName_.empty())
    {
        context_.animator.ChangeAnimation(idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                          context_.lowerBodyGroupName);
    }
}
}  // namespace Components
}  // namespace GameEngine
