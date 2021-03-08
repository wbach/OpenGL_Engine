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
    std::vector<uint32> jointsGroup;
};
}  // namespace Components
}  // namespace GameEngine
