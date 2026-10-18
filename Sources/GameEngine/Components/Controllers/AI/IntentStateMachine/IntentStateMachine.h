#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/DeadState.h"
#include "States/QuestState.h"
#include "States/ReactionState.h"
#include "States/RoutineState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using IntentStateMachine =
    Utils::StateMachine::Fsm<
                            RoutineState,
                            ReactionState,
                            QuestState,
                            DeadState
                            >;
}
// clang-format on
}  // namespace GameEngine
