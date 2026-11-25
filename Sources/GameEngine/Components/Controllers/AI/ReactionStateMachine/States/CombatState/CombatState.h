#pragma once
#include <Utils/Fsm/Actions.h>

#include <memory>

#include "../../ReactionAIEvents.h"
#include "CombatStateMachine.h"
#include "CombatStateContext.h"

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
                                       Utils::StateMachine::On<CombatStateEvent, Utils::StateMachine::Update>,
                                       Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    CombatState(ReactionStateContext&);
    CombatState(CombatState&&) = default;
    ~CombatState();

    void onEnter();
    void onEnter(const DamageTaken&);
    void onEnter(const TargetSpotted&);

    void update(float);
    void update(const CombatStateEvent&);

    void onLeave();
    void onLeave(const TargetLost&);
    void onLeave(const ThreatCleared&);

private:
    void onEnter(GameObject&);

private:
    ReactionStateContext& reactionStateContext;
    std::unique_ptr<CombatStateContext> combatStateContext;
    std::unique_ptr<CombatStateMachine> combatStateMachine;
};
}  // namespace Components
}  // namespace GameEngine
