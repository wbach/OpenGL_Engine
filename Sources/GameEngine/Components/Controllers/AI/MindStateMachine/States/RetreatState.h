#pragma once
#include <Utils/Fsm/Actions.h>

#include "../MindAIEvenets.h"

namespace GameEngine
{
namespace Components
{
class PassiveState;
class CombatState;
class PanicState;

class RetreatState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<PathBlocked, Utils::StateMachine::TransitionTo<CombatState>>,
                                       Utils::StateMachine::On<MoraleBroken, Utils::StateMachine::TransitionTo<PanicState>>,
                                       Utils::StateMachine::On<ThreatCleared, Utils::StateMachine::TransitionTo<PassiveState>>,
                                       Utils::StateMachine::On<TargetLost, Utils::StateMachine::TransitionTo<PassiveState>>,
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
