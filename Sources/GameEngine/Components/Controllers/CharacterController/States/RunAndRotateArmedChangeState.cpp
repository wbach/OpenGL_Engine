#include "RunAndRotateArmedChangeState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RunAndRotateArmedChangeState::RunAndRotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotatingMoveAndRotateStateBase{context,
                             context.lowerBodyGroupName,
                             context.runSpeed,
                             context.animClipNames.disarmed.run,
                             context.animClipNames.disarmed.rotateLeft,
                             context.animClipNames.disarmed.rotateRight}
    , context_{context}
{
}
void RunAndRotateArmedChangeState::onEnter()
{
    DEBUG_LOG("onEnter ");
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        DEBUG_LOG("onEnter 1");
        MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);
        RotateStateBase::updateAnimationClipNames(context_.animClipNames.armed.rotateLeft,
                                                  context_.animClipNames.armed.rotateRight);
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        DEBUG_LOG("onEnter 2");
        MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);
        RotateStateBase::updateAnimationClipNames(context_.animClipNames.disarmed.rotateLeft,
                                                  context_.animClipNames.disarmed.rotateRight);
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
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);
    ArmedChangeStateBase::update(e);
}

void RunAndRotateArmedChangeState::onEnter(ArmedRunAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);
}

void RunAndRotateArmedChangeState::onEnter(ArmedSprintAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);
}

void RunAndRotateArmedChangeState::onEnter(DisarmedSprintAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);
}

void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);
    ArmedChangeStateBase::update(e);
}

void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState &, const WeaponStateEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);
}
void RunAndRotateArmedChangeState::onEnter(RecoilWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);
}
void RunAndRotateArmedChangeState::onEnter(AimWalkAndRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);
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
