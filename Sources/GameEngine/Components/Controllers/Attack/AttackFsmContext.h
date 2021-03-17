#pragma once
#include <functional>
#include <optional>
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
struct AttackFsmContext
{
    Animator& animator;
    std::vector<std::string> attackAnimationNames;
    std::function<void()> nextAttackEvent;
    std::function<void()> sendEndEventCallback;

    std::optional<std::string> jointGroupName;
};
}  // namespace Components
}  // namespace GameEngine
