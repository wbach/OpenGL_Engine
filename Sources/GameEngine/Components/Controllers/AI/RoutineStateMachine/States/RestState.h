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

class RestState : public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<ShiftStarted, Utils::StateMachine::TransitionTo<WorkState>>,
                      Utils::StateMachine::On<HungerThresholdReached, Utils::StateMachine::TransitionTo<EatState>>,
                      Utils::StateMachine::On<FatigueThresholdReached, Utils::StateMachine::TransitionTo<SleepState>>>
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
