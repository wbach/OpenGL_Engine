#pragma once
#include <Utils/Fsm/Fsm.h>

#include "States/AimCrouchRotateState.h"
#include "States/AimCrouchState.h"
#include "States/AimCrouchWalkAndRotateState.h"
#include "States/AimCrouchWalkState.h"
#include "States/AimRotateState.h"
#include "States/AimState.h"
#include "States/AimWalkAndRotateState.h"
#include "States/AimWalkState.h"
#include "States/ArmedAttackAndRotateState.h"
#include "States/ArmedAttackAndRunAndRotateState.h"
#include "States/ArmedAttackAndRunState.h"
#include "States/ArmedAttackAndWalkAndRotateState.h"
#include "States/ArmedAttackAndWalkState.h"
#include "States/ArmedAttackState.h"
#include "States/ArmedCrouchRotateState.h"
#include "States/ArmedCrouchWalkAndRotateState.h"
#include "States/ArmedCrouchWalkState.h"
#include "States/ArmedFallingState.h"
#include "States/ArmedIdleCrouchState.h"
#include "States/ArmedIdleState.h"
#include "States/ArmedRotateState.h"
#include "States/ArmedRunAndRotateState.h"
#include "States/ArmedRunState.h"
#include "States/ArmedSprintAndRotateState.h"
#include "States/ArmedSprintState.h"
#include "States/ArmedWalkAndRotateState.h"
#include "States/ArmedWalkState.h"
#include "States/CrouchRotateArmedChangeState.h"
#include "States/CrouchWalkAndRotateArmedChangeState.h"
#include "States/CrouchWalkArmedChangeState.h"
#include "States/DeathState.h"
#include "States/DisarmedAttackAndRotateState.h"
#include "States/DisarmedAttackAndRunAndRotateState.h"
#include "States/DisarmedAttackAndRunState.h"
#include "States/DisarmedAttackAndWalkAndRotateState.h"
#include "States/DisarmedAttackAndWalkState.h"
#include "States/DisarmedAttackState.h"
#include "States/DisarmedCrouchRotateState.h"
#include "States/DisarmedCrouchWalkAndRotateState.h"
#include "States/DisarmedCrouchWalkState.h"
#include "States/DisarmedFallingState.h"
#include "States/DisarmedIdleCrouchState.h"
#include "States/DisarmedIdleState.h"
#include "States/DisarmedRotateState.h"
#include "States/DisarmedRunAndRotateState.h"
#include "States/DisarmedRunState.h"
#include "States/DisarmedSprintAndRotateState.h"
#include "States/DisarmedSprintState.h"
#include "States/DisarmedWalkAndRotateState.h"
#include "States/DisarmedWalkState.h"
#include "States/DrawArrowCrouchRotateState.h"
#include "States/DrawArrowCrouchState.h"
#include "States/DrawArrowCrouchWalkAndRotateState.h"
#include "States/DrawArrowCrouchWalkState.h"
#include "States/DrawArrowRotateState.h"
#include "States/DrawArrowState.h"
#include "States/DrawArrowWalkAndRotateState.h"
#include "States/DrawArrowWalkState.h"
#include "States/IdleArmedChangeState.h"
#include "States/IdleCrouchArmedChangeState.h"
#include "States/JumpState.h"
#include "States/DodgeState.h"
#include "States/RecoilCrouchRotateState.h"
#include "States/RecoilCrouchState.h"
#include "States/RecoilCrouchWalkAndRotateState.h"
#include "States/RecoilCrouchWalkState.h"
#include "States/RecoilRotateState.h"
#include "States/RecoilState.h"
#include "States/RecoilWalkAndRotateState.h"
#include "States/RecoilWalkState.h"
#include "States/RotateArmedChangeState.h"
#include "States/RunAndRotateArmedChangeState.h"
#include "States/RunArmedChangeState.h"
#include "States/WalkAndRotateArmedChangeState.h"
#include "States/WalkArmedChangeState.h"
#include "States/AnimatedTransitionState.h"

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
                            DisarmedAttackAndWalkAndRotateState,
                            DisarmedRunState,
                            DisarmedRotateState,
                            DisarmedCrouchRotateState,
                            DisarmedRunAndRotateState,
                            DisarmedWalkState,
                            DisarmedWalkAndRotateState,
                            DisarmedCrouchWalkAndRotateState,
                            DisarmedCrouchWalkState,
                            DisarmedSprintState,
                            DisarmedSprintAndRotateState,
                            IdleArmedChangeState,
                            IdleCrouchArmedChangeState,
                            RotateArmedChangeState,
                            CrouchRotateArmedChangeState,
                            RunArmedChangeState,
                            RunAndRotateArmedChangeState,
                            WalkArmedChangeState,
                            WalkAndRotateArmedChangeState,
                            CrouchWalkAndRotateArmedChangeState,
                            CrouchWalkArmedChangeState,
                            ArmedIdleState,
                            ArmedIdleCrouchState,
                            ArmedAttackState,
                            ArmedAttackAndRotateState,
                            ArmedAttackAndRunAndRotateState,
                            ArmedAttackAndWalkAndRotateState,
                            ArmedRunState,
                            ArmedRotateState,
                            ArmedCrouchRotateState,
                            ArmedRunAndRotateState,
                            ArmedWalkState,
                            ArmedWalkAndRotateState,
                            ArmedCrouchWalkAndRotateState,
                            ArmedCrouchWalkState,
                            ArmedSprintState,
                            ArmedSprintAndRotateState,
                            ArmedAttackAndRunState,
                            ArmedAttackAndWalkState,
                            JumpState,
                            DodgeState,
                            AimState,
                            AimCrouchState,
                            AimCrouchWalkState,
                            AimCrouchRotateState,
                            AimCrouchWalkAndRotateState,
                            AimRotateState,
                            AimWalkState,
                            AimWalkAndRotateState,
                            RecoilState,
                            RecoilCrouchState,
                            RecoilCrouchWalkState,
                            RecoilRotateState,
                            RecoilWalkState,
                            RecoilWalkAndRotateState,
                            RecoilCrouchRotateState,
                            RecoilCrouchWalkAndRotateState,
                            DrawArrowState,
                            DrawArrowCrouchState,
                            DrawArrowRotateState,
                            DrawArrowWalkState,
                            DrawArrowWalkAndRotateState,
                            DrawArrowCrouchWalkState,
                            DrawArrowCrouchRotateState,
                            DrawArrowCrouchWalkAndRotateState,
                            DisarmedFallingState,
                            ArmedFallingState,
                            DeathState,
                            AnimatedTransitionState
                            >;
}
// clang-format on
}  // namespace GameEngine
