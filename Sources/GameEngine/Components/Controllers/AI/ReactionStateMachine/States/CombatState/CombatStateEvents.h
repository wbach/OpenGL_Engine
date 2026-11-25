#pragma once
#include <Types.h>

#include <variant>

namespace GameEngine
{
class GameObject;
namespace Components
{
struct TargetInRangeAttackRangeEvent
{
    GameObject* target{nullptr};
};

struct TargetInMeleeRangeEvent
{
    GameObject* target{nullptr};
};

struct TargetOutOfRange
{
    GameObject* target{nullptr};
};

using CombatStateEvent = std::variant<TargetInMeleeRangeEvent, TargetInRangeAttackRangeEvent, TargetOutOfRange>;
}  // namespace Components
}  // namespace GameEngine
