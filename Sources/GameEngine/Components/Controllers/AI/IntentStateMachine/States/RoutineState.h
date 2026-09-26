#pragma once
#include <Utils/Fsm/Actions.h>

#include "../IntentStateEvents.h"

namespace GameEngine
{
namespace Components
{
class QuestState;
class DeadState;

class RoutineState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<QuestTriggered, Utils::StateMachine::TransitionTo<QuestState>>,
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
