#pragma once
#include <Utils/Fsm/Actions.h>

#include "../MindAIEvenets.h"

namespace GameEngine
{
namespace Components
{
class RoutineState;
class AttackState;
class RetreatState;

class PanicState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<ThreatCleared, Utils::StateMachine::TransitionTo<RoutineState>>,
                                       Utils::StateMachine::On<PathBlocked, Utils::StateMachine::TransitionTo<AttackState>>,
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
