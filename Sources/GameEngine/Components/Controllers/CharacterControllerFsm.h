#pragma once
#include <Utils/Fsm/Fsm.h>

#include "ArmedIdleState.h"
#include "ArmedMoveState.h"
#include "DeathState.h"
#include "IdleState.h"
#include "JumpState.h"
#include "MoveAndRotateState.h"
#include "MoveJumpState.h"
#include "MoveState.h"
#include "RotateState.h"

namespace GameEngine
{
namespace Components
{
using CharacterControllerFsm =
    Utils::StateMachine::Fsm<IdleState, MoveState, RotateState, MoveAndRotateState, JumpState,
                             MoveJumpState, ArmedIdleState, ArmedMoveState, DeathState>;
}
}  // namespace GameEngine
