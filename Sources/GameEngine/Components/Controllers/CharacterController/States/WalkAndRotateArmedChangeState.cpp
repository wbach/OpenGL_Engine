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
void WalkAndRotateArmedChangeState::onEnter()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.armed.walk);
        RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.rotateLeft,
                                                  context_.animClipNames.armed.rotateRight);
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
        MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);
        RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.rotateLeft,
                                                  context_.animClipNames.disarmed.rotateRight);
    }
}
void WalkAndRotateArmedChangeState::onEnter(DrawArrowWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
}

void WalkAndRotateArmedChangeState::onEnter(RecoilWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
}

void WalkAndRotateArmedChangeState::onEnter(AimWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
}

void WalkAndRotateArmedChangeState::update(float dt)
{
    MoveAndRotateStateBase::update(dt);
}

void WalkAndRotateArmedChangeState::onEnter(DisarmedWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.walk);
}

void WalkAndRotateArmedChangeState::onEnter(ArmedWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
}

void WalkAndRotateArmedChangeState::onEnter(DisarmedWalkAndRotateState&, const DrawArrowEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.walk);
    ArmedChangeStateBase::update(e);
}
void WalkAndRotateArmedChangeState::update(const WeaponStateEvent & e)
{
    ArmedChangeStateBase::update(e);
    onEnter();
}
}  // namespace Components
}  // namespace GameEngine
