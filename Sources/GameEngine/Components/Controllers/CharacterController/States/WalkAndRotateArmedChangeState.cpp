#include "WalkAndRotateArmedChangeState.h"


namespace GameEngine
{
namespace Components
{
WalkAndRotateArmedChangeState::WalkAndRotateArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context,
                             context.lowerBodyGroupName,
                             context.walkSpeed,
                             context.animClipNames.disarmed.walk,
                             context.animClipNames.disarmed.rotateLeft,
                             context.animClipNames.disarmed.rotateRight}
    , context_{context}
{
}

void WalkAndRotateArmedChangeState::onEnter(DrawArrowWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void WalkAndRotateArmedChangeState::onEnter(RecoilWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void WalkAndRotateArmedChangeState::onEnter(AimWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void WalkAndRotateArmedChangeState::update(float dt)
{
    MoveAndRotateStateBase::update(dt);
}

void WalkAndRotateArmedChangeState::onEnter(DisarmedWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}

void WalkAndRotateArmedChangeState::onEnter(ArmedWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void WalkAndRotateArmedChangeState::onEnter(DisarmedWalkAndRotateState&, const DrawArrowEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
}

}  // namespace Components
}  // namespace GameEngine
