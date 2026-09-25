#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/RoutineState.h"
#include "States/QuestState.h"
#include "States/DialogState.h"
#include "States/SuspiciousState.h"
#include "States/AttackState.h"
#include "States/SurrenderedState.h"
#include "States/PanicState.h"
#include "States/RetreatState.h"
#include "States/DeadState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using MindStateMachine =
    Utils::StateMachine::Fsm<
                            RoutineState,
                            QuestState,
                            DialogState,
                            SuspiciousState,
                            AttackState,
                            SurrenderedState,
                            PanicState,
                            RetreatState,
                            DeadState
                            >;
}
// clang-format on
}  // namespace GameEngine
