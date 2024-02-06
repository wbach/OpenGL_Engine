#include "RunAndRotateArmedChangeState.h"
#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RunAndRotateArmedChangeState::RunAndRotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context,
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
    drawArrowEventCalled_ = false;
    sprintEventCalled_ = false;
}
void RunAndRotateArmedChangeState::onEnter(const SprintStartEvent& event)
{
    MoveStateBase::onEnter(event);
    sprintEventCalled_ = true;
}
void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState&, const DrawArrowEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void RunAndRotateArmedChangeState::onEnter(ArmedRunAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
}
void RunAndRotateArmedChangeState::update(float dt)
{
    MoveAndRotateStateBase::update(dt);
}
void RunAndRotateArmedChangeState::update(const DrawArrowEvent&)
{
    drawArrowEventCalled_ = true;
}
void RunAndRotateArmedChangeState::update(const SprintStateChangeEvent&)
{
    sprintEventCalled_ = not sprintEventCalled_;
}
void RunAndRotateArmedChangeState::update(const AimStopEvent&)
{
    drawArrowEventCalled_ = false;
}
void RunAndRotateArmedChangeState::onLeave(const EquipEndStateEvent& e)
{
    DEBUG_LOG("onLeave " + std::to_string(drawArrowEventCalled_));
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
