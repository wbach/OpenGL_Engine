#include "CrouchWalkAndRotateArmedChangeState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
CrouchWalkAndRotateArmedChangeState::CrouchWalkAndRotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotatingMoveAndRotateStateBase{context, context.lowerBodyGroupName, context.walkSpeed,
                                     context.animClipNames.disarmed.movement.crouch,
                                     context.animClipNames.disarmed.posture.crouched.rotate}
    , context_{context}
{
}
void CrouchWalkAndRotateArmedChangeState::onEnter()
{
    // /*DISABLED*/ DEBUG_LOG("onEnter ");
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        // /*DISABLED*/ DEBUG_LOG("onEnter 1");
        MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.crouch);
        MoveStateBase::setCurrentAnimIfNeeded();
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.crouch);
        MoveStateBase::setCurrentAnimIfNeeded();
    }
}

void CrouchWalkAndRotateArmedChangeState::onEnter(const SprintStateChangeEvent &event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}

void CrouchWalkAndRotateArmedChangeState::onEnter(DisarmedCrouchWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
}

void CrouchWalkAndRotateArmedChangeState::onEnter(DisarmedSprintAndRotateState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.crouch);
    ArmedChangeStateBase::update(e);
}

void CrouchWalkAndRotateArmedChangeState::onEnter(ArmedRunAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.crouch);
}

void CrouchWalkAndRotateArmedChangeState::onEnter(ArmedSprintAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.crouch);
}

void CrouchWalkAndRotateArmedChangeState::onEnter(DisarmedSprintAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.crouch);
}

void CrouchWalkAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.crouch);
    ArmedChangeStateBase::update(e);
}

void CrouchWalkAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState &, const WeaponStateEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.crouch);
}
void CrouchWalkAndRotateArmedChangeState::onEnter(RecoilWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.crouch);
}
void CrouchWalkAndRotateArmedChangeState::onEnter(AimWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.crouch);
}
void CrouchWalkAndRotateArmedChangeState::update(float dt)
{
    RotatingMoveAndRotateStateBase::update(dt);
}

void CrouchWalkAndRotateArmedChangeState::update(const WeaponStateEvent &e)
{
    ArmedChangeStateBase::update(e);
    onEnter();
}
}  // namespace Components
}  // namespace GameEngine
