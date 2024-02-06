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

void RunArmedChangeState::onEnter(const SprintStartEvent& event)
{
    MoveStateBase::onEnter(event);
    queuedEvent = event;
}

void RunArmedChangeState::update(float dt)
{
    MoveStateBase::update(dt);
}
void RunArmedChangeState::update(const SprintStateChangeEvent& event)
{
    MoveStateBase::onEnter(event);
    queuedEvent = event;
}
void RunArmedChangeState::onLeave(const EquipEndStateEvent&)
{
    if (queuedEvent)
    {
        context_.characterController.pushEventToQueue(*queuedEvent);
        queuedEvent.reset();
    }
}
}  // namespace Components
}  // namespace GameEngine
