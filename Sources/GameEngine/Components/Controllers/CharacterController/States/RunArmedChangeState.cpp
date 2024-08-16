#include "RunArmedChangeState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
namespace GameEngine
{
namespace Components
{
RunArmedChangeState::RunArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotatingMoveState{context, context.lowerBodyGroupName, context.runSpeed.forward,
                        context.animClipNames.disarmed.movement.run.forward}
    , context_{context}
{
}

void RunArmedChangeState::onEnter()
{
    // /*DISABLED*/ DEBUG_LOG("RunArmedChangeState onEnter");
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
    }
    // /*DISABLED*/ DEBUG_LOG("End on enter");
}

void RunArmedChangeState::onEnter(const SprintStateChangeEvent& event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}

void RunArmedChangeState::onEnter(DisarmedRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
}
void RunArmedChangeState::onEnter(ArmedRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
}
void RunArmedChangeState::onEnter(DisarmedRunState&, const DrawArrowEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
    ArmedChangeStateBase::update(e);
}

void RunArmedChangeState::onEnter(DisarmedSprintState&, const DrawArrowEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
    ArmedChangeStateBase::update(e);
}

void RunArmedChangeState::onEnter(DrawArrowWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
}

void RunArmedChangeState::onEnter(RecoilWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
}

void RunArmedChangeState::onEnter(AimWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
}

void RunArmedChangeState::onEnter(DisarmedSprintState&, const WeaponStateEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
}

void RunArmedChangeState::onEnter(ArmedSprintState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
}

void RunArmedChangeState::update(float dt)
{
    RotatingMoveState::update(dt);
}

void RunArmedChangeState::update(const SprintStateChangeEvent& event)
{
    ArmedChangeStateBase::update(event);
}

void RunArmedChangeState::update(const WeaponStateEvent& e)
{
    ArmedChangeStateBase::update(e);
    onEnter();
}

void RunArmedChangeState::onMoveInactivity()
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
