#pragma once
#include <vector>
#include <functional>
#include "AnimationClipInfo.h"

namespace GameEngine
{
namespace Components
{
struct TransitionGroupsPlaying
{
    const AnimationClipInfo& info;
    float currentTime;
    std::vector<std::string> jointGroupNames;
    std::function<void()> onTransitionEnd;
};
}  // namespace Components
}  // namespace GameEngine
