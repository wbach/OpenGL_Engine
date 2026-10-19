#pragma once
#include <Utils/Fsm/Actions.h>

#include "../ReactionAIEvents.h"

namespace GameEngine
{
namespace Components
{
class PassiveState;
class CombatState;
class PanicState;
class SuspiciousState;

class PassiveState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<DamageTaken, Utils::StateMachine::TransitionTo<CombatState>>,
                                       Utils::StateMachine::On<TargetSpotted, Utils::StateMachine::TransitionTo<CombatState>>, // CombatState PanicState or retreat state, w zaleznosci jak od postaci?
                                       Utils::StateMachine::On<NoiseHeard, Utils::StateMachine::TransitionTo<SuspiciousState>>,
                                       Utils::StateMachine::On<MoraleBroken, Utils::StateMachine::TransitionTo<PanicState>>,
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
