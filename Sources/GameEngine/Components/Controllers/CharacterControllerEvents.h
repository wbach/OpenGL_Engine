#pragma once
#include <Types.h>

namespace GameEngine
{
struct MoveForwardEvent
{
};
struct MoveBackwardEvent
{
};
struct MoveLeftEvent
{
};
struct MoveRightEvent
{
};
struct WalkEvent
{
};
struct RunEvent
{
};
struct CrouchEvent
{
};
struct SprintEvent
{
};
struct EndMoveEvent
{
};
struct RotateLeftEvent
{
};
struct RotateRightEvent
{
};
struct RotateTargetEvent
{
    Quaternion target;
};
struct EndRotationEvent
{
};
struct JumpEvent
{
    float power{1.f};
};
struct EndJumpEvent
{
};
struct AttackEvent
{
};
struct NextAttackEvent
{
};
struct EndAttackEvent
{
};
struct DeathEvent
{
};
struct WeaponStateEvent
{
};
}  // namespace GameEngine
