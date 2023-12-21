#pragma once
#include <Utils/Fsm/Fsm.h>

#include "IdleState.h"
#include "MoveState.h"
#include "RotateState.h"
#include "JumpState.h"
#include "MoveAndRotateState.h"
#include "MoveJumpState.h"
#include "DeathState.h"
#include "IdleStateWithWeapon.h"

namespace GameEngine
{
namespace Components
{
using CharacterControllerFsm = Utils::StateMachine::Fsm<IdleState, IdleStateWithWeapon, MoveState, RotateState, MoveAndRotateState, JumpState, MoveJumpState, DeathState>;
}
}  // namespace GameEngine
