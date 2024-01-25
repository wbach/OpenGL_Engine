#pragma once
#include <Utils/Fsm/Fsm.h>

#include "ArmedIdleState.h"
#include "ArmedRotateState.h"
#include "ArmedRunAndRotateState.h"
#include "ArmedRunState.h"
#include "ArmedSprintAndRotateState.h"
#include "ArmedSprintState.h"
#include "ArmedWalkAndRotateState.h"
#include "ArmedWalkState.h"
#include "DeathState.h"
#include "DisarmedIdleState.h"
#include "DisarmedRotateState.h"
#include "DisarmedRunAndRotateState.h"
#include "DisarmedRunState.h"
#include "DisarmedSprintAndRotateState.h"
#include "DisarmedSprintState.h"
#include "DisarmedWalkAndRotateState.h"
#include "DisarmedWalkState.h"
#include "JumpState.h"
#include "MoveJumpState.h"
#include "AimState.h"
#include "RecoilState.h"
#include "DrawArrowState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
struct FsmContext;
using CharacterControllerFsm =
    Utils::StateMachine::Fsm<
                            DisarmedIdleState,
                            DisarmedRunState,
                            DisarmedRotateState,
                            DisarmedRunAndRotateState,
                            DisarmedWalkState,
                            DisarmedWalkAndRotateState,
                            DisarmedSprintState,
                            DisarmedSprintAndRotateState,
                            ArmedIdleState,
                            ArmedRunState,
                            ArmedRotateState,
                            ArmedRunAndRotateState,
                            ArmedWalkState,
                            ArmedWalkAndRotateState,
                            ArmedSprintState,
                            ArmedSprintAndRotateState,
                            JumpState,
                            MoveJumpState,
                            AimState,
                            RecoilState,
                            DrawArrowState,
                            DeathState
                            >;
}
// clang-format on
}  // namespace GameEngine
