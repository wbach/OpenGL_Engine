#pragma once

#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"


namespace GameEngine
{
namespace Components
{
struct FsmContext;

class AnimatedTransitionState;
class DisarmedIdleState;
class DisarmedAttackState;
class DisarmedIdleCrouchState;
class CrouchWalkArmedChangeState;
class CrouchRotateArmedChangeState;
class CrouchWalkAndRotateArmedChangeState;
class DisarmedAttackAndWalkState;
class DisarmedAttackAndRunState;
class DisarmedAttackAndRotateState;
class DisarmedAttackAndRunAndRotateState;
class DisarmedAttackAndWalkAndRotateState;
class DisarmedRunState;
class DisarmedRotateState;
class DisarmedCrouchRotateState;
class DisarmedRunAndRotateState;
class DisarmedWalkState;
class DisarmedCrouchWalkState;
class DisarmedWalkAndRotateState;
class DisarmedCrouchWalkAndRotateState;
class DisarmedSprintState;
class DisarmedSprintAndRotateState;
class IdleArmedChangeState;
class IdleCrouchArmedChangeState;
class RotateArmedChangeState;
class RunArmedChangeState;
class RunAndRotateArmedChangeState;
class WalkArmedChangeState;
class WalkAndRotateArmedChangeState;
class ArmedIdleState;
class ArmedIdleCrouchState;
class ArmedAttackState;
class ArmedRunState;
class ArmedRotateState;
class ArmedCrouchRotateState;
class ArmedRunAndRotateState;
class ArmedAttackAndRotateState;
class ArmedAttackAndRunAndRotateState;
class ArmedAttackAndWalkAndRotateState;
class ArmedAttackAndWalkState;
class ArmedAttackAndRunState;
class ArmedWalkState;
class ArmedWalkAndRotateState;
class ArmedCrouchWalkAndRotateState;
class ArmedCrouchWalkState;
class ArmedSprintState;
class ArmedSprintAndRotateState;
class JumpState;
class DodgeState;
class AimState;
class AimCrouchState;
class AimCrouchWalkState;
class AimRotateState;
class AimCrouchRotateState;
class AimWalkState;
class AimWalkAndRotateState;
class AimCrouchWalkAndRotateState;
class RecoilState;
class RecoilCrouchState;
class RecoilRotateState;
class RecoilCrouchRotateState;
class RecoilWalkState;
class RecoilCrouchWalkState;
class RecoilWalkAndRotateState;
class RecoilCrouchWalkAndRotateState;
class DrawArrowState;
class DrawArrowRotateState;
class DrawArrowCrouchRotateState;
class DrawArrowWalkState;
class DrawArrowWalkAndRotateState;
class DrawArrowCrouchWalkAndRotateState;
class DrawArrowCrouchState;
class DrawArrowCrouchWalkState;
class DisarmedFallingState;
class ArmedFallingState;
class DeathState;
}  // namespace Components
}  // namespace GameEngine
