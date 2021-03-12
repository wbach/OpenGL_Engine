#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "AttackFsmContext.h"

namespace GameEngine
{
namespace Components
{
class EmptyState;

class AttackState : public Utils::StateMachine::Will<
                        Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                        Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<EmptyState>>>
{
public:
    AttackState(AttackFsmContext& context)
        : context_{context}
    {
    }
    void onEnter()
    {
    }

private:
    AttackFsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
