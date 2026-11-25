#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CombatStateContext.h"
#include "../CombatStateEvents.h"

namespace GameEngine
{
namespace Components
{
struct EndAttackEvent;

class AttackState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    AttackState(CombatStateContext&);

    void onEnter();
    void onEnter(const TargetInMeleeRangeEvent&);
    void onEnter(const TargetInRangeAttackRangeEvent&);

    void update(float);

    void onLeave();

private:
    void process(const EndAttackEvent&);

private:
    CombatStateContext& combatStateContext;
    GameObject* target{nullptr};
    IdType characterControllerEventSubId;
};
}  // namespace Components
}  // namespace GameEngine
