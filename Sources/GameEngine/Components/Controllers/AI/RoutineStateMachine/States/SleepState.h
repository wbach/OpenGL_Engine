#pragma once
#include <Utils/Fsm/Actions.h>

#include "../RoutineAIEvents.h"

namespace GameEngine
{
namespace Components
{
class WorkState;
class RestState;

class SleepState
    : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<WakeUp, Utils::StateMachine::TransitionTo<RestState>>,
                                       Utils::StateMachine::On<ShiftStarted, Utils::StateMachine::TransitionTo<WorkState>>>
{
public:
    void onEnter()
    {
    }
    void update(float dt)
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
