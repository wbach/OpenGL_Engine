#pragma once
#include <Types.h>

#include <variant>
#include "PlayStateType.h"

namespace GameEngine
{
struct MoveEvent
{
};
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
struct WalkChangeStateEvent
{
};
struct RunEvent
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
struct EndMoveLeftEvent
{
};
struct EndMoveRightEvent
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
struct ChangeAnimEvent
{
    std::string name;
    Components::PlayStateType stateType;
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
struct CrouchChangeStateEvent
{
};
using CharacterControllerEvent =
    std::variant<MoveEvent, MoveForwardEvent, MoveBackwardEvent, MoveLeftEvent, MoveRightEvent, WalkChangeStateEvent,
                 RunEvent, SprintStateChangeEvent, EndForwardMoveEvent, EndBackwardMoveEvent, RotateLeftEvent,
                 RotateRightEvent, RotateTargetEvent, EndRotationEvent, JumpEvent, EndJumpEvent, AttackEvent,
                 NextAttackEvent, EndAttackEvent, DeathEvent, WeaponStateEvent, EquipEndStateEvent, DisarmEndStateEvent,
                 DrawArrowEvent, ReloadArrowEvent, AimStartEvent, AimStopEvent, EndMoveLeftEvent, EndMoveRightEvent,
                 CrouchChangeStateEvent, ChangeAnimEvent>;
}  // namespace GameEngine
