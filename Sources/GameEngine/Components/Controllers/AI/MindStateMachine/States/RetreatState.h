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

class RetreatState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<PathBlocked, Utils::StateMachine::TransitionTo<AttackState>>,
                                       Utils::StateMachine::On<MoraleBroken, Utils::StateMachine::TransitionTo<PanicState>>,
                                       Utils::StateMachine::On<ThreatCleared, Utils::StateMachine::TransitionTo<RoutineState>>,
                                       Utils::StateMachine::On<TargetLost, Utils::StateMachine::TransitionTo<RoutineState>>,
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
