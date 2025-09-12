#include "RunAndRotateArmedChangeState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
RunAndRotateArmedChangeState::RunAndRotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotatingMoveAndRotateStateBase{context, context.lowerBodyGroupName, context.runSpeed,
                                     context.animClipNames.disarmed.movement.run,
                                     context.animClipNames.disarmed.posture.stand.rotate}
    , context_{context}
{
}
void RunAndRotateArmedChangeState::onEnter()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
        MoveStateBase::setCurrentAnimIfNeeded();
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
        MoveStateBase::setCurrentAnimIfNeeded();
    }
}

void RunAndRotateArmedChangeState::onEnter(const SprintStateChangeEvent &event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}

void RunAndRotateArmedChangeState::onEnter(DisarmedSprintAndRotateState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
    ArmedChangeStateBase::update(e);
}

void RunAndRotateArmedChangeState::onEnter(ArmedRunAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
}

void RunAndRotateArmedChangeState::onEnter(ArmedSprintAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
}

void RunAndRotateArmedChangeState::onEnter(DisarmedSprintAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
}

void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
    ArmedChangeStateBase::update(e);
}

void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState &, const WeaponStateEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.run);
}
void RunAndRotateArmedChangeState::onEnter(RecoilWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
}
void RunAndRotateArmedChangeState::onEnter(AimWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.run);
}
void RunAndRotateArmedChangeState::update(float dt)
{
    RotatingMoveAndRotateStateBase::update(dt);
}

void RunAndRotateArmedChangeState::update(const WeaponStateEvent &e)
{
    ArmedChangeStateBase::update(e);
    onEnter();
}
}  // namespace Components
}  // namespace GameEngine
