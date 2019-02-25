#pragma once
#include <vector>
#include "Types.h"

namespace GameEngine
{
struct TerrainConfiguration
{
    vec3 scale = vec3(6000, 0, 6000);
    std::vector<uint32> lodRanges_ = {1750, 874, 386, 192, 100, 0, 0};
    std::vector<uint32> loadMorphingAreas_;
};
}  // namespace GameEngine
