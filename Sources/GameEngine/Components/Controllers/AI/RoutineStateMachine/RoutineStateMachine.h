#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/WorkState.h"
#include "States/EatState.h"
#include "States/RestState.h"
#include "States/SleepState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using RoutineStateMachine =
    Utils::StateMachine::Fsm<
                            WorkState,
                            RestState,
                            EatState,
                            SleepState
                            >;
}
// clang-format on
}  // namespace GameEngine
