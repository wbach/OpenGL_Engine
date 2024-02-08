#include "RunAndRotateArmedChangeState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RunAndRotateArmedChangeState::RunAndRotateArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context,
                             context.lowerBodyGroupName,
                             context.runSpeed,
                             context.animClipNames.disarmed.run,
                             context.animClipNames.disarmed.rotateLeft,
                             context.animClipNames.disarmed.rotateRight}
    , context_{context}
{
}
void RunAndRotateArmedChangeState::onEnter(const SprintStartEvent& event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}
void RunAndRotateArmedChangeState::onEnter(const SprintStateChangeEvent& event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}

void RunAndRotateArmedChangeState::onEnter(DisarmedSprintAndRotateState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
    MoveStateBase::updateMoveState();
}
void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState&, const DrawArrowEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void RunAndRotateArmedChangeState::onEnter(ArmedRunAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void RunAndRotateArmedChangeState::onEnter(ArmedSprintAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void RunAndRotateArmedChangeState::onEnter(DisarmedSprintAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::updateMoveState();
}
void RunAndRotateArmedChangeState::onEnter(DrawArrowRunAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}
void RunAndRotateArmedChangeState::onEnter(RecoilRunAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}
void RunAndRotateArmedChangeState::onEnter(AimRunAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}
void RunAndRotateArmedChangeState::update(float dt)
{
    MoveAndRotateStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
