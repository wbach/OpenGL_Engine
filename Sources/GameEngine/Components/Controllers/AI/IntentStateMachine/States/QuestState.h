#pragma once
#include <Utils/Fsm/Actions.h>

#include "../IntentStateEvents.h"

namespace GameEngine
{
namespace Components
{
class RoutineState;
class DeadState;

class QuestState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<QuestEnded, Utils::StateMachine::TransitionTo<RoutineState>>,
                                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeadState>>,
                                       Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    void onEnter();
    void update(float)
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
