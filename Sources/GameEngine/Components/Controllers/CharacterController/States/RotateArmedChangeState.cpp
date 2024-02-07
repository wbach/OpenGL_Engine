#include "RotateArmedChangeState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RotateArmedChangeState::RotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.leftRight,
                      context.animClipNames.disarmed.rotateLeft, context.animClipNames.disarmed.rotateRight}
    , context_{context}
{
}

void RotateArmedChangeState::onEnter(DisarmedRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
}

void RotateArmedChangeState::onEnter(ArmedRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}

void RotateArmedChangeState::onEnter(DisarmedRotateState &, const DrawArrowEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    context_.drawArrowEventCalled_ = true;
}

void RotateArmedChangeState::update(float dt)
{
    RotateStateBase::update(dt);
}

void RotateArmedChangeState::update(const DrawArrowEvent &)
{
    context_.drawArrowEventCalled_ = true;
}

void RotateArmedChangeState::update(const AimStopEvent &)
{
    context_.drawArrowEventCalled_ = false;
}

void RotateArmedChangeState::update(const RotateRightEvent &e)
{
    RotateStateBase::onEnter(e);
}

void RotateArmedChangeState::update(const RotateLeftEvent &e)
{
    RotateStateBase::onEnter(e);
}

void RotateArmedChangeState::update(const RotateTargetEvent &e)
{
    RotateStateBase::onEnter(e);
}

}  // namespace Components
}  // namespace GameEngine
