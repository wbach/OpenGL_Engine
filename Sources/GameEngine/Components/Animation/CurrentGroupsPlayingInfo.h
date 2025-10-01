#pragma once
#include <vector>
#include <string>

namespace GameEngine
{
namespace Components
{
struct AnimationClipInfo;

struct CurrentGroupsPlayingInfo
{
    const AnimationClipInfo& info;
    float currentTime;
    std::vector<std::string> jointGroupNames;
};
}  // namespace Components
}  // namespace GameEngine
