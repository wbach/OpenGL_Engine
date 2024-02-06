#pragma once
#include <Types.h>

#include <variant>

namespace GameEngine
{
struct WalkForwardEvent
{
};
struct WalkBackwardEvent
{
};
struct WalkChangeStateEvent
{
};
struct RunForwardEvent
{
};
struct RunBackwardEvent
{
};
struct MoveLeftEvent
{
};
struct MoveRightEvent
{
};
struct RunEvent
{
};
struct SprintStartEvent
{
};
struct SprintStateChangeEvent
{
};
struct EndForwardMoveEvent
{
};
struct EndBackwardMoveEvent
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
struct WeaponChangeEndEvent  // to remove
{
};
struct EquipEndStateEvent
{
};
struct DisarmEndStateEvent
{
};
struct DrawArrowEvent
{
};
struct ReloadArrowEvent
{
};
struct AimStartEvent
{
};
struct AimStopEvent
{
};

using CharacterControllerEvent =
    std::variant<WalkForwardEvent, WalkBackwardEvent, WalkChangeStateEvent, RunForwardEvent, RunBackwardEvent,
                 MoveLeftEvent, MoveRightEvent, RunEvent, SprintStartEvent, SprintStateChangeEvent,
                 EndForwardMoveEvent, EndBackwardMoveEvent, RotateLeftEvent, RotateRightEvent, RotateTargetEvent,
                 EndRotationEvent, JumpEvent, EndJumpEvent, AttackEvent, NextAttackEvent, EndAttackEvent, DeathEvent,
                 WeaponStateEvent, EquipEndStateEvent, DisarmEndStateEvent, DrawArrowEvent, ReloadArrowEvent,
                 AimStartEvent, AimStopEvent>;
}  // namespace GameEngine
