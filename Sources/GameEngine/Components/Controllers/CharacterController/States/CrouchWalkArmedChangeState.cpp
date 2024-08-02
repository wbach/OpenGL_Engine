#include "CrouchWalkArmedChangeState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
CrouchWalkArmedChangeState::CrouchWalkArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotatingMoveState{context, context.lowerBodyGroupName, context.walkSpeed.forward,
                        context.animClipNames.disarmed.crouch.forward}
    , context_{context}
{
}
void CrouchWalkArmedChangeState::onEnter()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.armed.crouch);
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.crouch);
    }
}
void CrouchWalkArmedChangeState::onEnter(DisarmedCrouchWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.crouch);
}
void CrouchWalkArmedChangeState::onEnter(ArmedCrouchWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.crouch);
}
void CrouchWalkArmedChangeState::onEnter(DisarmedCrouchWalkState&, const DrawArrowEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.crouch);
    ArmedChangeStateBase::update(e);
}
void CrouchWalkArmedChangeState::onEnter(DrawArrowCrouchWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.crouch);
}
void CrouchWalkArmedChangeState::onEnter(RecoilCrouchWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.crouch);
}
void CrouchWalkArmedChangeState::onEnter(AimCrouchWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.crouch);
}
void CrouchWalkArmedChangeState::update(float dt)
{
    RotatingMoveState::update(dt);
}
void CrouchWalkArmedChangeState::update(const WeaponStateEvent& e)
{
    ArmedChangeStateBase::update(e);
    onEnter();
}

void CrouchWalkArmedChangeState::onMoveInactivity()
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
