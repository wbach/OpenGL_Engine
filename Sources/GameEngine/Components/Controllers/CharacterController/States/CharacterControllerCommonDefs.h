#pragma once

#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;

class DisarmedIdleState;
class DisarmedAttackState;
class DisarmedIdleCrouchState;
class DisarmedAttackAndWalkState;
class DisarmedAttackAndRunState;
class DisarmedAttackAndRotateState;
class DisarmedAttackAndRunAndRotateState;
class DisarmedRunState;
class DisarmedRotateState;
class DisarmedRunAndRotateState;
class DisarmedWalkState;
class DisarmedWalkAndRotateState;
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
class ArmedRunAndRotateState;
class ArmedWalkState;
class ArmedWalkAndRotateState;
class ArmedSprintState;
class ArmedSprintAndRotateState;
class JumpState;
class MoveJumpState;
class AimState;
class AimRotateState;
class AimWalkState;
class AimWalkAndRotateState;
class RecoilState;
class RecoilRotateState;
class RecoilWalkState;
class RecoilWalkAndRotateState;
class DrawArrowState;
class DrawArrowRotateState;
class DrawArrowWalkState;
class DrawArrowWalkAndRotateState;
class DeathState;
}  // namespace Components
}  // namespace GameEngine
