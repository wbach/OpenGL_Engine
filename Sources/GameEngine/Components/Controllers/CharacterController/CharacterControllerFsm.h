#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/AimRotateState.h"
#include "States/AimState.h"
#include "States/AimWalkAndRotateState.h"
#include "States/AimWalkState.h"
#include "States/ArmedAttackState.h"
#include "States/ArmedIdleState.h"
#include "States/ArmedRotateState.h"
#include "States/ArmedRunAndRotateState.h"
#include "States/ArmedRunState.h"
#include "States/ArmedSprintAndRotateState.h"
#include "States/ArmedSprintState.h"
#include "States/ArmedWalkAndRotateState.h"
#include "States/ArmedWalkState.h"
#include "States/DeathState.h"
#include "States/DisarmedAttackAndRunState.h"
#include "States/DisarmedAttackAndWalkState.h"
#include "States/DisarmedAttackState.h"
#include "States/DisarmedIdleState.h"
#include "States/DisarmedRotateState.h"
#include "States/DisarmedRunAndRotateState.h"
#include "States/DisarmedRunState.h"
#include "States/DisarmedSprintAndRotateState.h"
#include "States/DisarmedSprintState.h"
#include "States/DisarmedWalkAndRotateState.h"
#include "States/DisarmedWalkState.h"
#include "States/DrawArrowRotateState.h"
#include "States/DrawArrowState.h"
#include "States/DrawArrowWalkAndRotateState.h"
#include "States/DrawArrowWalkState.h"
#include "States/IdleArmedChangeState.h"
#include "States/JumpState.h"
#include "States/MoveJumpState.h"
#include "States/RecoilRotateState.h"
#include "States/RecoilState.h"
#include "States/RecoilWalkAndRotateState.h"
#include "States/RecoilWalkState.h"
#include "States/RotateArmedChangeState.h"
#include "States/RunAndRotateArmedChangeState.h"
#include "States/RunArmedChangeState.h"
#include "States/WalkAndRotateArmedChangeState.h"
#include "States/WalkArmedChangeState.h"
#include "States/DisarmedAttackAndRotateState.h"
#include "States/DisarmedAttackAndRunAndRotateState.h"
#include "States/DisarmedIdleCrouchState.h"
#include "States/ArmedIdleCrouchState.h"
#include "States/IdleCrouchArmedChangeState.h"
#include "States/DisarmedCrouchWalkState.h"
#include "States/CrouchWalkArmedChangeState.h"
#include "States/ArmedCrouchWalkState.h"
#include "States/AimCrouchState.h"
#include "States/DrawArrowCrouchState.h"
#include "States/RecoilCrouchState.h"
#include "States/DrawArrowCrouchWalkState.h"
#include "States/AimCrouchWalkState.h"
#include "States/RecoilCrouchWalkState.h"
#include "States/ArmedAttackAndRunState.h"
#include "States/ArmedAttackAndWalkState.h"

namespace GameEngine
{
namespace Components
{
// clang-format off
using CharacterControllerFsm =
    Utils::StateMachine::Fsm<
                            DisarmedIdleState,
                            DisarmedIdleCrouchState,
                            DisarmedAttackState,
                            DisarmedAttackAndRunState,
                            DisarmedAttackAndWalkState,
                            DisarmedAttackAndRotateState,
                            DisarmedAttackAndRunAndRotateState,
                            DisarmedRunState,
                            DisarmedRotateState,
                            DisarmedRunAndRotateState,
                            DisarmedWalkState,
                            DisarmedWalkAndRotateState,
                            DisarmedCrouchWalkState,
                            DisarmedSprintState,
                            DisarmedSprintAndRotateState,
                            IdleArmedChangeState,
                            IdleCrouchArmedChangeState,
                            RotateArmedChangeState,
                            RunArmedChangeState,
                            RunAndRotateArmedChangeState,
                            WalkArmedChangeState,
                            WalkAndRotateArmedChangeState,
                            CrouchWalkArmedChangeState,
                            ArmedIdleState,
                            ArmedIdleCrouchState,
                            ArmedAttackState,
                            ArmedRunState,
                            ArmedRotateState,
                            ArmedRunAndRotateState,
                            ArmedWalkState,
                            ArmedWalkAndRotateState,
                            ArmedCrouchWalkState,
                            ArmedSprintState,
                            ArmedSprintAndRotateState,
                            ArmedAttackAndRunState,
                            ArmedAttackAndWalkState,
                            JumpState,
                            MoveJumpState,
                            AimState,
                            AimCrouchState,
                            AimCrouchWalkState,
                            AimRotateState,
                            AimWalkState,
                            AimWalkAndRotateState,
                            RecoilState,
                            RecoilCrouchState,
                            RecoilCrouchWalkState,
                            RecoilRotateState,
                            RecoilWalkState,
                            RecoilWalkAndRotateState,
                            DrawArrowState,
                            DrawArrowCrouchState,
                            DrawArrowRotateState,
                            DrawArrowWalkState,
                            DrawArrowWalkAndRotateState,
                            DrawArrowCrouchWalkState,
                            DeathState
                            >;
}
// clang-format on
}  // namespace GameEngine
