#include "BaseState.h"

#include "../FsmContext.h"
#include "../CharacterController.h"

namespace GameEngine::Components
{
BaseState::BaseState(FsmContext &context)
    : context_{context}
    , queue_{context.eventsQueue}
{
}
void BaseState::update(float)
{
}
void BaseState::flushEvents()
{
    for (const auto &e : queue_)
    {
        context_.characterController.pushEventToQueue(e);
    }
    queue_.clear();
}
}  // namespace GameEngine::Components
