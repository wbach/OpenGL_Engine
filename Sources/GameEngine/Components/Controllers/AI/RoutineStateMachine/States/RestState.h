#pragma once
#include <Utils/Fsm/Actions.h>

#include "../RoutineAIEvents.h"

namespace GameEngine
{
namespace Components
{
class WorkState;
class EatState;
class SleepState;
class SuspendState;

class RestState : public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<WorkStarted, Utils::StateMachine::TransitionTo<WorkState>>,
                      Utils::StateMachine::On<MealStarted, Utils::StateMachine::TransitionTo<EatState>>,
                      Utils::StateMachine::On<SleepStarted, Utils::StateMachine::TransitionTo<SleepState>>,
                      Utils::StateMachine::On<Suspend, Utils::StateMachine::TransitionTo<SuspendState>>,
                      Utils::StateMachine::On<Resume, Utils::StateMachine::TransitionTo<RestState>>,
                      Utils::StateMachine::On<RestStarted, Utils::StateMachine::TransitionTo<RestState>>>
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
