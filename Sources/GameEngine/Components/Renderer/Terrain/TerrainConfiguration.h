#pragma once
#include <vector>
#include "Types.h"

namespace GameEngine
{
struct TerrainConfiguration
{
    TerrainConfiguration() 
        : scale(6000, 0, 6000)
    {
        lodRanges_ = { 1750, 874, 386, 192, 100, 0, 0 };
    }
    vec3 scale;
    std::vector<uint32> lodRanges_;
    std::vector<uint32> loadMorphingAreas_;
};
}  // namespace GameEngine
