#include "WalkArmedChangeState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
WalkArmedChangeState::WalkArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotatingMoveState{context, context.lowerBodyGroupName, context.walkSpeed.forward, context.animClipNames.disarmed.walk.forward}
    , context_{context}
{
}
void WalkArmedChangeState::onEnter()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.armed.walk);
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
    }
}
void WalkArmedChangeState::onEnter(DisarmedWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.walk);
}
void WalkArmedChangeState::onEnter(ArmedWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
}
void WalkArmedChangeState::onEnter(DisarmedWalkState&, const DrawArrowEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.walk);
    ArmedChangeStateBase::update(e);
}
void WalkArmedChangeState::onEnter(DrawArrowWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
}
void WalkArmedChangeState::onEnter(RecoilWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
}
void WalkArmedChangeState::onEnter(AimWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
}
void WalkArmedChangeState::update(float dt)
{
    RotatingMoveState::update(dt);
}
void WalkArmedChangeState::update(const WeaponStateEvent& e)
{
    ArmedChangeStateBase::update(e);
    onEnter();
}

void WalkArmedChangeState::onMoveInactivity()
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
