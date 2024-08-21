#include "CrouchWalkAndRotateArmedChangeState.h"

#include "../FsmContext.h"
#include "RotateStateBase.h"

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

void CrouchWalkAndRotateArmedChangeState::onEnter(WalkAndRotateArmedChangeState &prevState, const CrouchChangeStateEvent &)
{
    if (prevState.getCurrentAnimationClips().left == context_.animClipNames.armed.posture.stand.rotate.left)
    {
        RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.posture.crouched.rotate);
    }
}

void CrouchWalkAndRotateArmedChangeState::onEnter(DisarmedCrouchWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.posture.crouched.rotate);
}

void CrouchWalkAndRotateArmedChangeState::onEnter(DisarmedSprintAndRotateState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.crouch);
    ArmedChangeStateBase::update(e);
}

void CrouchWalkAndRotateArmedChangeState::onEnter(ArmedCrouchWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.crouch);
}

void CrouchWalkAndRotateArmedChangeState::onEnter(DisarmedCrouchWalkAndRotateState &, const DrawArrowEvent & e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.crouch);
    ArmedChangeStateBase::update(e);
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

void CrouchWalkAndRotateArmedChangeState::onEnter(RecoilCrouchWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.crouch);
}
void CrouchWalkAndRotateArmedChangeState::onEnter(DrawArrowCrouchWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.crouch);
}
void CrouchWalkAndRotateArmedChangeState::onEnter(AimCrouchWalkAndRotateState &, const WeaponStateEvent &)
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
