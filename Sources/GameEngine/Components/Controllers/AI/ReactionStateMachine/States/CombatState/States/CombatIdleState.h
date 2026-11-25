#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CombatStateEvents.h"

namespace GameEngine
{
namespace Components
{
class AttackState;
class ChaseState;

class CombatIdleState : public Utils::StateMachine::Will<
                            Utils::StateMachine::On<TargetInMeleeRangeEvent, Utils::StateMachine::TransitionTo<AttackState>>,
                            Utils::StateMachine::On<TargetInRangeAttackRangeEvent, Utils::StateMachine::TransitionTo<AttackState>>,
                            Utils::StateMachine::On<TargetOutOfRange, Utils::StateMachine::TransitionTo<ChaseState>>,
                            Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    CombatIdleState()
    {
    }

    void onEnter()
    {
    }
    void update(float)
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
