#include "CrouchRotateArmedChangeState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
CrouchRotateArmedChangeState::CrouchRotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotateStateBase{context, context.lowerBodyGroupName, context.walkSpeed.rotate,
                      context.animClipNames.disarmed.posture.crouched.rotate, RotateStateBase::CameraRotationPolicy::none}
    , context_{context}
{
}
void CrouchRotateArmedChangeState::onEnter()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.posture.crouched.rotate);
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.crouched.rotate);
    }
}

void CrouchRotateArmedChangeState::onEnter(DisarmedCrouchRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.posture.crouched.rotate);
    RotateStateBase::setCurrentAnim();
}

void CrouchRotateArmedChangeState::onEnter(DisarmedCrouchRotateState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.posture.crouched.rotate);
    RotateStateBase::setCurrentAnim();
}

void CrouchRotateArmedChangeState::onEnter(ArmedCrouchRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.crouched.rotate);
    RotateStateBase::setCurrentAnimAndRotation();
}

void CrouchRotateArmedChangeState::onEnter(DrawArrowCrouchRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.crouched.rotate);
    RotateStateBase::setCurrentAnimAndRotation();
}

void CrouchRotateArmedChangeState::onEnter(RecoilCrouchRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.crouched.rotate);
    RotateStateBase::setCurrentAnimAndRotation();
}

void CrouchRotateArmedChangeState::onEnter(AimCrouchRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.crouched.rotate);
    RotateStateBase::setCurrentAnimAndRotation();
}

void CrouchRotateArmedChangeState::update(float dt)
{
    RotateStateBase::update(dt);
}

void CrouchRotateArmedChangeState::update(const RotateRightEvent &e)
{
    RotateStateBase::onEnter(e);
}

void CrouchRotateArmedChangeState::update(const RotateLeftEvent &e)
{
    RotateStateBase::onEnter(e);
}

void CrouchRotateArmedChangeState::update(const RotateTargetEvent &e)
{
    RotateStateBase::onEnter(e);
}

}  // namespace Components
}  // namespace GameEngine
