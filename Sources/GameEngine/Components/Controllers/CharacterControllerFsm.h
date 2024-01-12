#pragma once
#include <Utils/Fsm/Fsm.h>

#include "ArmedIdleState.h"
#include "ArmedRotateState.h"
#include "ArmedRunAndRotateState.h"
#include "ArmedRunState.h"
#include "DeathState.h"
#include "DisarmedIdleState.h"
#include "DisarmedRotateState.h"
#include "DisarmedRunAndRotateState.h"
#include "DisarmedRunState.h"
#include "JumpState.h"
#include "MoveJumpState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using CharacterControllerFsm =
    Utils::StateMachine::Fsm<
                            DisarmedIdleState,
                            DisarmedRunState,
                            DisarmedRotateState,
                            DisarmedRunAndRotateState,
                            ArmedIdleState,
                            ArmedRunState,
                            ArmedRotateState,
                            ArmedRunAndRotateState,
                            JumpState,
                            MoveJumpState,
                            DeathState
                            >;
}
// clang-format on
}  // namespace GameEngine
