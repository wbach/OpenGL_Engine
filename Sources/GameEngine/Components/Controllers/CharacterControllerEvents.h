#pragma once
#include <Types.h>

namespace GameEngine
{
struct MoveForwardEvent
{
    float moveSpeed{1.f};
};
struct MoveBackwardEvent
{
    float moveSpeed{1.f};
};
struct EndMoveEvent
{
};
struct RotateLeftEvent
{
    float speed{1.f};
};
struct RotateRightEvent
{
    float speed{1.f};
};
struct RotateTargetEvent
{
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
}  // namespace GameEngine
