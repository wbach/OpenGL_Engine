#pragma once
#include <Utils/Fsm/Actions.h>

#include "../../ReactionStateMachine/ReactionAIEvents.h"
#include "../IntentStateEvents.h"

namespace GameEngine
{
namespace Components
{
class QuestState;
class DeadState;
class ReactionState;

class RoutineComponent;

class RoutineState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<QuestTriggered, Utils::StateMachine::TransitionTo<QuestState>>,
                                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeadState>>,
                                       Utils::StateMachine::On<ReactionAIEvent, Utils::StateMachine::TransitionTo<ReactionState>>,
                                       Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    RoutineState(RoutineComponent*);

    void onEnter();
    void onLeave();
    void update(float);

private:
    RoutineComponent* routineComponent_{nullptr};
};
}  // namespace Components
}  // namespace GameEngine
