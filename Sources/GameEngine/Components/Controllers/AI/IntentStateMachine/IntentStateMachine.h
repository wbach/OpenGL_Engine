#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/RoutineState.h"
#include "States/QuestState.h"
#include "States/DeadState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using IntentStateMachine =
    Utils::StateMachine::Fsm<
                            RoutineState,
                            QuestState,
                            DeadState
                            >;
}
// clang-format on
}  // namespace GameEngine
