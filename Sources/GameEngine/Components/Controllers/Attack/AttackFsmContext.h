#pragma once
#include "GameEngine/Components/Animation/Animator.h"
#include <functional>

namespace GameEngine
{
namespace Components
{
struct AttackFsmContext
{
    Animator& animator;
    const std::string& attackAnimationName;
    std::function<void()> nextAttackEvent;
    std::function<void ()> sendEndEventCallback;
};
}  // namespace Components
}  // namespace GameEngine
