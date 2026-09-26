#pragma once
#include <Utils/Fsm/Actions.h>

#include "../MindAIEvenets.h"

namespace GameEngine
{
namespace Components
{
class RoutineState;
class CombatState;
class PanicState;
class DialogState;

class SurrenderedState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<DamageTaken, Utils::StateMachine::TransitionTo<CombatState>>,
                                       Utils::StateMachine::On<MoraleBroken, Utils::StateMachine::TransitionTo<PanicState>>,
                                       Utils::StateMachine::On<DialogueStarted, Utils::StateMachine::TransitionTo<DialogState>>,
                                       Utils::StateMachine::On<ThreatCleared, Utils::StateMachine::TransitionTo<RoutineState>>,
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
