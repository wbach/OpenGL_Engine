#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/AIAmbientState.h"
#include "States/AIMoveToState.h"
#include "States/AIReturnState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using ActionStateMachine =
    Utils::StateMachine::Fsm<
                            AIAmbientState,
                            AIReturnState,
                            AIMoveToState
                            >;
}
// clang-format on
}  // namespace GameEngine
