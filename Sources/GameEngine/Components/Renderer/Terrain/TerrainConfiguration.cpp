#include "TerrainConfiguration.h"

namespace GameEngine
{
TerrainConfiguration::TerrainConfiguration()
    : scaleXZ_(6000)
    , scaleY_(40)
{
    SetLod(0, 1750);
    SetLod(1, 874);
    SetLod(2, 386);
    SetLod(3, 192);
    SetLod(4, 100);
    SetLod(5, 50);
    SetLod(6, 0);
    SetLod(7, 0);
}
int32 TerrainConfiguration::updateMorphingArea(uint32 lod)
{
    return static_cast<uint32>((scaleXZ_ / static_cast<float>(TerrainDef::TERRAIN_ROOT_NODES_COUNT)) /
                               static_cast<float>(pow(2, lod)));
}
void TerrainConfiguration::SetLod(uint32 index, uint32 value)
{
    if (index >= lodRanges_.size())
    {
        lodRanges_.resize(index + 1);
        morphingAreas_.resize(index + 1);
    }

    lodRanges_[index]     = value;
    morphingAreas_[index] = value - updateMorphingArea(index + 1);
}
}  // namespace GameEngine
