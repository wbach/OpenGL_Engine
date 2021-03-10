#pragma once
#include <Utils/Fsm/Fsm.h>

#include "IdleState.h"
#include "MoveState.h"

namespace GameEngine
{
namespace Components
{
using CharacterControllerFsm = Utils::StateMachine::Fsm<IdleState, MoveState>;
}
}  // namespace GameEngine
