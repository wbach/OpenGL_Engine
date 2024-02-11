#include "WalkArmedChangeState.h"

namespace GameEngine
{
namespace Components
{
WalkArmedChangeState::WalkArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.disarmed.walk}
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
    MoveStateBase::updateMoveState();
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
    MoveStateBase::updateMoveState();
}
void WalkArmedChangeState::onEnter(RecoilWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
    MoveStateBase::updateMoveState();
}
void WalkArmedChangeState::onEnter(AimWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
    MoveStateBase::updateMoveState();
}
void WalkArmedChangeState::update(float dt)
{
    MoveStateBase::update(dt);
}
void WalkArmedChangeState::update(const WeaponStateEvent & e)
{
    ArmedChangeStateBase::update(e);
    onEnter();
}
}  // namespace Components
}  // namespace GameEngine
