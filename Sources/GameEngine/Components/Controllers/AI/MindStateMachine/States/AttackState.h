#pragma once
#include <Utils/Fsm/Actions.h>

#include "../MindAIEvenets.h"

namespace GameEngine
{
namespace Components
{
class PanicState;
class RoutineState;
class QuestState;
class RetreatState;

class AttackState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<LowHealthReached, Utils::StateMachine::TransitionTo<RetreatState>>,
                                       Utils::StateMachine::On<MoraleBroken, Utils::StateMachine::TransitionTo<PanicState>>,
                                       Utils::StateMachine::On<PathBlocked, Utils::StateMachine::TransitionTo<PanicState>>,
                                       Utils::StateMachine::On<ThreatCleared, Utils::StateMachine::TransitionTo<RoutineState>>,
                                       Utils::StateMachine::On<TargetLost, Utils::StateMachine::TransitionTo<RoutineState>>,
                                       Utils::StateMachine::On<DialogueStarted, Utils::StateMachine::TransitionTo<QuestState>>,
                                       Utils::StateMachine::On<QuestTriggered, Utils::StateMachine::TransitionTo<QuestState>>,
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
