#include "WalkAndRotateArmedChangeState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
WalkAndRotateArmedChangeState::WalkAndRotateArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotatingMoveAndRotateStateBase{context,
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
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.walk);
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.rotateLeft,
                                              context_.animClipNames.armed.rotateRight);
}

void WalkAndRotateArmedChangeState::setDisarmAnimations()
{
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.walk);
    RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.rotateLeft,
                                              context_.animClipNames.disarmed.rotateRight);
}
}  // namespace Components
}  // namespace GameEngine
