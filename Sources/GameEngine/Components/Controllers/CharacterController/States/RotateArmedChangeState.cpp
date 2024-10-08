#include "RotateArmedChangeState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
RotateArmedChangeState::RotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.disarmed.posture.stand.rotate, RotateStateBase::CameraRotationPolicy::none}
    , context_{context}
{
}
void RotateArmedChangeState::onEnter()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.posture.stand.rotate);
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.stand.rotate);
    }
}
void RotateArmedChangeState::onEnter(DisarmedRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.posture.stand.rotate);
    RotateStateBase::setCurrentAnim();
}

void RotateArmedChangeState::onEnter(ArmedRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.stand.rotate);
    RotateStateBase::setCurrentAnimAndRotation();
}

void RotateArmedChangeState::onEnter(DisarmedRotateState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.posture.stand.rotate);
    RotateStateBase::setCurrentAnim();
}

void RotateArmedChangeState::onEnter(DrawArrowRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.stand.rotate);
    RotateStateBase::setCurrentAnimAndRotation();
}

void RotateArmedChangeState::onEnter(RecoilRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.stand.rotate);
    RotateStateBase::setCurrentAnimAndRotation();
}

void RotateArmedChangeState::onEnter(AimRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.stand.rotate);
    RotateStateBase::setCurrentAnimAndRotation();
}

void RotateArmedChangeState::update(float dt)
{
    RotateStateBase::update(dt);
}

void RotateArmedChangeState::update(const RotateRightEvent &e)
{
    RotateStateBase::onEnter(e);
}

void RotateArmedChangeState::update(const RotateLeftEvent &e)
{
    RotateStateBase::onEnter(e);
}

void RotateArmedChangeState::update(const RotateTargetEvent &e)
{
    RotateStateBase::onEnter(e);
}

}  // namespace Components
}  // namespace GameEngine
