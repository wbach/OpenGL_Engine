#pragma once
#include <optional>

#include "AnimationClipInfo.h"

namespace GameEngine
{
namespace Components
{
struct ChangeAnimationEvent
{
    float startTime{0.f};
    const AnimationClipInfo& info;
    std::optional<std::string> jointGroupName;
};
}  // namespace Components
}  // namespace GameEngine
