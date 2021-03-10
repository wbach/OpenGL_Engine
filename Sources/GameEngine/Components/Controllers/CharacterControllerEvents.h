#pragma once
#include <Types.h>

namespace GameEngine
{
struct MoveForwardEvent
{
    float moveSpeed{1.f};
    vec3 direction{0.f, 0.f, 1.f};
};
struct MoveBackwardEvent
{
    float moveSpeed{1.f};
    vec3 direction{0.f, 0.f, -1.f};
};
struct EndMoveEvent
{
};
struct RotateLeftEvent
{
};
struct EndRotateLeftEvent
{
};
struct RotateRightEvent
{
};
struct EndRotateRightEvent
{
};
struct RotateTargetEvent
{
};
struct JumpEvent
{
};
struct AttackEvent
{
};
}  // namespace GameEngine
