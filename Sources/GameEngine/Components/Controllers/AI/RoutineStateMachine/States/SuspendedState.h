#pragma once
#include <Utils/Fsm/Actions.h>

#include "../RoutineAIEvents.h"

namespace GameEngine
{
namespace Components
{
class WorkState;
class RestState;
class EatState;
class SleepState;
class SuspendState;

class SuspendedState : public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<WorkStarted, Utils::StateMachine::TransitionTo<WorkState>>,
                      Utils::StateMachine::On<MealStarted, Utils::StateMachine::TransitionTo<EatState>>,
                      Utils::StateMachine::On<SleepStarted, Utils::StateMachine::TransitionTo<SleepState>>,
                      Utils::StateMachine::On<Resume, Utils::StateMachine::TransitionTo<WorkState>>,
                      Utils::StateMachine::On<RestStarted, Utils::StateMachine::TransitionTo<RestState>>>
{
public:
    void update(float){}
};
}  // namespace Components
}  // namespace GameEngine
