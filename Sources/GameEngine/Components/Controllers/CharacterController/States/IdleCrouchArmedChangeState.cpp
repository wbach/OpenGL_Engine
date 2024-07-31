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
    , IdleStateBase{context, context.animClipNames.armed.crouchIdle}
    , context_{context}
{
}

void IdleCrouchArmedChangeState::onEnter(const CrouchChangeStateEvent &)
{
}

void IdleCrouchArmedChangeState::onEnter(IdleArmedChangeState &, const CrouchChangeStateEvent &)
{
    idleAnimName_ = context_.animClipNames.armed.crouchIdle;
    setAnim();
}
void IdleCrouchArmedChangeState::onEnter(DisarmedIdleCrouchState &, const WeaponStateEvent &)
{
    // /*DISABLED*/ DEBUG_LOG("onEnter(DisarmedIdleCrouchState&, const WeaponStateEvent &)");
    idleAnimName_ = context_.animClipNames.armed.crouchIdle;
    setAnim();
    ArmedChangeStateBase::equipWeapon();
}
void IdleCrouchArmedChangeState::onEnter(ArmedIdleCrouchState &, const WeaponStateEvent &)
{
    // /*DISABLED*/ DEBUG_LOG("onEnter(ArmedIdleCrouchState&, const WeaponStateEvent &)");
    idleAnimName_ = context_.animClipNames.disarmed.crouchIdle;
    setAnim();
    ArmedChangeStateBase::disarmWeapon();
}
void IdleCrouchArmedChangeState::onEnter(DisarmedIdleCrouchState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
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
