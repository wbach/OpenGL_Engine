#pragma once
#include <functional>
#include <optional>
#include <string>

namespace GameEngine
{
namespace Components
{
struct AnimationClipInfo;
struct ChangeAnimationEvent
{
    float startTime{0.f};
    const AnimationClipInfo& info;
    std::optional<std::string> jointGroupName;
    std::function<void()> onTransitionEnd{nullptr};
};
}  // namespace Components
}  // namespace GameEngine
