#pragma once
#include <functional>
#include <optional>

#include "GameEngine/Components/Controllers/AnimationClipNames.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
struct AttackFsmContext
{
    Animator& animator;
    const AnimationClipsNames& animClipNames;
    std::function<void()> sendEndEventCallback;

    std::optional<std::string> jointGroupName;
};
}  // namespace Components
}  // namespace GameEngine
