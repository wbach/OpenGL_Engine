#include "RunArmedChangeState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RunArmedChangeState::RunArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.disarmed.run.forward,
                    context.animClipNames.disarmed.run.backward}
    , context_{context}
{
}

void RunArmedChangeState::onEnter()
{
    drawArrowEventCalled_ = false;
    sprintEventCalled_ = false;
}

void RunArmedChangeState::onEnter(const SprintStartEvent& event)
{
    MoveStateBase::onEnter(event);
    sprintEventCalled_ = true;
}

void RunArmedChangeState::onEnter(DisarmedIdleState&, const DrawArrowEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}

void RunArmedChangeState::onEnter(DisarmedIdleState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}

void RunArmedChangeState::onEnter(ArmedIdleState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
}

void RunArmedChangeState::update(float dt)
{
    MoveStateBase::update(dt);
}
void RunArmedChangeState::update(const DrawArrowEvent& event)
{
    drawArrowEventCalled_ = true;
}
void RunArmedChangeState::update(const SprintStateChangeEvent& event)
{
    MoveStateBase::onEnter(event);
    sprintEventCalled_ = true;
}
void RunArmedChangeState::update(const AimStopEvent&)
{
    drawArrowEventCalled_ = false;
}
void RunArmedChangeState::onLeave(const EquipEndStateEvent& e)
{
    if (drawArrowEventCalled_)
    {
        DEBUG_LOG("pushEventToQueue");
        context_.characterController.pushEventToQueue(DrawArrowEvent{});
    }
    else if (sprintEventCalled_)
    {
        context_.characterController.pushEventToQueue(SprintStateChangeEvent{});
    }
    ArmedChangeStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
