#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/PassiveState.h"
#include "States/SuspiciousState.h"
#include "States/CombatState.h"
#include "States/SurrenderedState.h"
#include "States/PanicState.h"
#include "States/RetreatState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using MindStateMachine =
    Utils::StateMachine::Fsm<
                            PassiveState,
                            SuspiciousState,
                            CombatState,
                            SurrenderedState,
                            PanicState,
                            RetreatState
                            >;
}
// clang-format on
}  // namespace GameEngine
