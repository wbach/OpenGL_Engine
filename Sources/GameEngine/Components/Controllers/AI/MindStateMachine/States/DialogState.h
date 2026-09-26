#pragma once
#include <Utils/Fsm/Actions.h>

#include "../MindAIEvenets.h"

namespace GameEngine
{
namespace Components
{
class RoutineState;
class AttackState;
class PanicState;
class QuestState;

class DialogState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<DialogueEnded, Utils::StateMachine::TransitionTo<RoutineState>>,
                                       Utils::StateMachine::On<DamageTaken, Utils::StateMachine::TransitionTo<AttackState>>,
                                       Utils::StateMachine::On<MoraleBroken, Utils::StateMachine::TransitionTo<PanicState>>,
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
