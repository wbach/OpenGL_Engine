#include "RunArmedChangeState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RunArmedChangeState::RunArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.disarmed.run.forward,
                    context.animClipNames.disarmed.run.backward}
    , context_{context}
{
}
void RunArmedChangeState::onEnter(const SprintStartEvent& event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}
void RunArmedChangeState::onEnter(const SprintStateChangeEvent& event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}
void RunArmedChangeState::onEnter(DisarmedRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void RunArmedChangeState::onEnter(ArmedRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}
void RunArmedChangeState::onEnter(DisarmedRunState&, const DrawArrowEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
}

void RunArmedChangeState::onEnter(DisarmedSprintState &, const DrawArrowEvent & e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
    MoveStateBase::updateMoveState();
}

void RunArmedChangeState::onEnter(DrawArrowRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void RunArmedChangeState::onEnter(RecoilRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void RunArmedChangeState::onEnter(AimRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void RunArmedChangeState::onEnter(DisarmedSprintState &, const WeaponStateEvent & e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::updateMoveState();
}

void RunArmedChangeState::onEnter(ArmedSprintState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::updateMoveState();
}

void RunArmedChangeState::update(float dt)
{
    MoveStateBase::update(dt);
}

void RunArmedChangeState::update(const SprintStateChangeEvent& event)
{
    ArmedChangeStateBase::update(event);
}
}  // namespace Components
}  // namespace GameEngine
