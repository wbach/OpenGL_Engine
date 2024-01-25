#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/ArmedIdleState.h"
#include "States/ArmedRotateState.h"
#include "States/ArmedRunAndRotateState.h"
#include "States/ArmedRunState.h"
#include "States/ArmedSprintAndRotateState.h"
#include "States/ArmedSprintState.h"
#include "States/ArmedWalkAndRotateState.h"
#include "States/ArmedWalkState.h"
#include "States/DeathState.h"
#include "States/DisarmedIdleState.h"
#include "States/DisarmedRotateState.h"
#include "States/DisarmedRunAndRotateState.h"
#include "States/DisarmedRunState.h"
#include "States/DisarmedSprintAndRotateState.h"
#include "States/DisarmedSprintState.h"
#include "States/DisarmedWalkAndRotateState.h"
#include "States/DisarmedWalkState.h"
#include "States/JumpState.h"
#include "States/MoveJumpState.h"
#include "States/AimState.h"
#include "States/RecoilState.h"
#include "States/DrawArrowState.h"

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
