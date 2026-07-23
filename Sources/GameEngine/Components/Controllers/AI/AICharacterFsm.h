#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/AIAmbientState.h"
#include "States/AIAttackState.h"
#include "States/AIChaseState.h"
#include "States/AIMoveToState.h"
#include "States/AIReturnState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using AICharacterFsm =
    Utils::StateMachine::Fsm<
                            AIAmbientState,
                            AIChaseState,
                            AIReturnState,
                            AIAttackState,
                            AIMoveToState
                            >;
}
// clang-format on
}  // namespace GameEngine
