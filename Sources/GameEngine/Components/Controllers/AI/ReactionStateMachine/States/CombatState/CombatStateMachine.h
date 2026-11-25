#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/AttackState.h"
#include "States/ChaseState.h"
#include "States/CombatIdleState.h"
#include "States/DefenseState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using CombatStateMachine =
    Utils::StateMachine::Fsm<
        CombatIdleState,
        ChaseState,
        AttackState,
        DefenseState
    >;
}
// clang-format on
}  // namespace GameEngine
