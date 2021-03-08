#pragma once
#include <vector>
#include "AnimationClipInfo.h"

namespace GameEngine
{
namespace Components
{
struct CurrentGroupsPlayingInfo
{
    const AnimationClipInfo& info;
    float currentTime;
    std::vector<std::string> jointGroupNames;
};
}  // namespace Components
}  // namespace GameEngine
