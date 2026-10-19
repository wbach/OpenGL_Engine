#include "RoutineState.h"

#include "../../RoutineComponent.h"
#include "../../RoutineStateMachine/RoutineAIEvents.h"

namespace GameEngine
{
namespace Components
{
RoutineState::RoutineState(RoutineComponent* routineComponent)
    : routineComponent_(routineComponent)
{
    if (not routineComponent_)
    {
        LOG_ERROR << "RoutineComponent is null";
    }
}
void RoutineState::onEnter()
{
    if (routineComponent_)
    {
        routineComponent_->HandleEvent(Resume{});
    }
}
void RoutineState::update(float)
{
}
void RoutineState::onLeave()
{
    if (routineComponent_)
    {
        routineComponent_->HandleEvent(Suspend{});
    }
}
}  // namespace Components
}  // namespace GameEngine
