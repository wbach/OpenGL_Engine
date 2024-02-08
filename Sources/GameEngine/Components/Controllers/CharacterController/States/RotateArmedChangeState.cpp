#include "RotateArmedChangeState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RotateArmedChangeState::RotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.leftRight,
                      context.animClipNames.disarmed.rotateLeft, context.animClipNames.disarmed.rotateRight}
    , context_{context}
{
}

void RotateArmedChangeState::onEnter(DisarmedRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
}

void RotateArmedChangeState::onEnter(ArmedRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::setCurrentAnimAndRotation();
}

void RotateArmedChangeState::onEnter(DisarmedRotateState &, const DrawArrowEvent & e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
}

void RotateArmedChangeState::onEnter(DrawArrowRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::setCurrentAnimAndRotation();
}

void RotateArmedChangeState::onEnter(RecoilRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    RotateStateBase::setCurrentAnimAndRotation();
}

void RotateArmedChangeState::onEnter(AimRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
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
