#pragma once
#include <Utils/Fsm/Fsm.h>

#include "IdleState.h"
#include "MoveState.h"
#include "RotateState.h"
#include "JumpState.h"
#include "MoveAndRotateState.h"
#include "MoveJumpState.h"

namespace GameEngine
{
namespace Components
{
using CharacterControllerFsm = Utils::StateMachine::Fsm<IdleState, MoveState, RotateState, MoveAndRotateState, JumpState, MoveJumpState>;
}
}  // namespace GameEngine
