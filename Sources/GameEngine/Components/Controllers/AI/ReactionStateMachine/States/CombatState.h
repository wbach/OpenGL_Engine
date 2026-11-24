#pragma once
#include <Utils/Fsm/Actions.h>

#include "../ReactionAIEvents.h"

namespace GameEngine
{
namespace Components
{
class PanicState;
class RetreatState;
class PassiveState;
class ReactionStateContext;
class CombatState
    : public Utils::StateMachine::Will<Utils::StateMachine::On<LowHealthReached, Utils::StateMachine::TransitionTo<RetreatState>>,
                                       Utils::StateMachine::On<MoraleBroken, Utils::StateMachine::TransitionTo<PanicState>>,
                                       Utils::StateMachine::On<PathBlocked, Utils::StateMachine::TransitionTo<PanicState>>,
                                       Utils::StateMachine::On<ThreatCleared, Utils::StateMachine::TransitionTo<PassiveState>>,
                                       Utils::StateMachine::On<TargetLost, Utils::StateMachine::TransitionTo<PassiveState>>,
                                       Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    CombatState(ReactionStateContext&);

    void onEnter();
    void update(float);

    void onLeave(const TargetLost&);
    void onLeave(const ThreatCleared&);

private:
    ReactionStateContext& reactionStateContext;
};
}  // namespace Components
}  // namespace GameEngine
