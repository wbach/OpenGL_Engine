#include "WalkAndRotateArmedChangeState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
WalkAndRotateArmedChangeState::WalkAndRotateArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotatingMoveAndRotateStateBase{context, context.lowerBodyGroupName, context.walkSpeed,
                                     context.animClipNames.disarmed.movement.walk,
                                     context.animClipNames.disarmed.posture.stand.rotate}
    , context_{context}
{
}
void WalkAndRotateArmedChangeState::onEnter()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        setEquipAnimations();
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        setDisarmAnimations();
    }
}
void WalkAndRotateArmedChangeState::onEnter(DrawArrowWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.walk);
}

void WalkAndRotateArmedChangeState::onEnter(RecoilWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.walk);
}

void WalkAndRotateArmedChangeState::onEnter(AimWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.walk);
}

void WalkAndRotateArmedChangeState::update(float dt)
{
    RotatingMoveAndRotateStateBase::update(dt);
}

void WalkAndRotateArmedChangeState::onEnter(DisarmedWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
    setEquipAnimations();
}

void WalkAndRotateArmedChangeState::onEnter(ArmedWalkAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    setDisarmAnimations();
}

void WalkAndRotateArmedChangeState::onEnter(DisarmedWalkAndRotateState&, const DrawArrowEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    setEquipAnimations();
    ArmedChangeStateBase::update(e);
}
void WalkAndRotateArmedChangeState::update(const WeaponStateEvent& e)
{
    ArmedChangeStateBase::update(e);
    onEnter();
}

void WalkAndRotateArmedChangeState::setEquipAnimations()
{
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.movement.walk);
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.posture.stand.rotate);
}

void WalkAndRotateArmedChangeState::setDisarmAnimations()
{
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.movement.walk);
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.posture.stand.rotate);
}
}  // namespace Components
}  // namespace GameEngine
