#pragma once
#include <functional>
#include <vector>
#include <string>

namespace GameEngine
{
namespace Components
{
struct AnimationClipInfo;
struct TransitionGroupsPlaying
{
    const AnimationClipInfo& info;
    float currentTime;
    std::vector<std::string> jointGroupNames;
    std::function<void()> onTransitionEnd;
};
}  // namespace Components
}  // namespace GameEngine
