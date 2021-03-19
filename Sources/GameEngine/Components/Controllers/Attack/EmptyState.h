#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "AttackEvents.h"
#include "AttackFsmContext.h"

namespace GameEngine
{
namespace Components
{
class AttackState;

struct EmptyState
    : public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackFsmEvents::Attack, Utils::StateMachine::TransitionTo<AttackState>>,
          Utils::StateMachine::On<AttackFsmEvents::AttackGrouped, Utils::StateMachine::TransitionTo<AttackState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<EmptyState>>>
{
};
}  // namespace Components
}  // namespace GameEngine
