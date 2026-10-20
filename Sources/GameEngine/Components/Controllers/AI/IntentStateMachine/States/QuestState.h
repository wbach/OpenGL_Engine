#pragma once
#include <Utils/Fsm/Actions.h>

#include "../../ReactionStateMachine/ReactionAIEvents.h"
#include "../IntentStateEvents.h"

namespace GameEngine
{
namespace Components
{
class RoutineState;
class DeadState;
class ReactionState;

class QuestState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<QuestEnded, Utils::StateMachine::TransitionTo<RoutineState>>,
                                       Utils::StateMachine::On<EntityDiedEvent, Utils::StateMachine::TransitionTo<DeadState>>,
                                       Utils::StateMachine::On<ReactionAIEvent, Utils::StateMachine::TransitionTo<ReactionState>>,
                                       Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    void onEnter()
    {
    }
    void update(float)
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
