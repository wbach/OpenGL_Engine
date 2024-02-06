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

void RotateArmedChangeState::onEnter()
{
    drawArrowEventCalled_ = false;
}

void RotateArmedChangeState::onEnter(DisarmedRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::equipWeapon();
}

void RotateArmedChangeState::onEnter(ArmedRotateState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
}

void RotateArmedChangeState::onEnter(DisarmedRotateState&, const DrawArrowEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    drawArrowEventCalled_ = true;
}

void RotateArmedChangeState::update(float dt)
{
    RotateStateBase::update(dt);
}

void RotateArmedChangeState::update(const DrawArrowEvent &)
{
    drawArrowEventCalled_ = true;
}

void RotateArmedChangeState::update(const AimStopEvent &)
{
    drawArrowEventCalled_ = false;
}

void RotateArmedChangeState::onLeave(const EquipEndStateEvent & e)
{
    DEBUG_LOG("onLeave " + std::to_string(drawArrowEventCalled_));
    if (drawArrowEventCalled_)
    {
        DEBUG_LOG("pushEventToQueue");
        ArmedChangeStateBase::context_.characterController.pushEventToQueue(DrawArrowEvent{});
    }
    ArmedChangeStateBase::onLeave(e);
}

}  // namespace Components
}  // namespace GameEngine
