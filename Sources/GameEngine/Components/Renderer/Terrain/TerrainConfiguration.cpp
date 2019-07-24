#include "TerrainConfiguration.h"

namespace GameEngine
{
const size_t LOD_SIZE = 8;

TerrainConfiguration::TerrainConfiguration()
    : perTerrainBuffer{vec4(0.3), vec4i(0), vec4i(0), vec3(6000, 800, 6000)}
{
    lodRanges_.resize(LOD_SIZE);
    SetLod(0, 1750);
    SetLod(1, 874);
    SetLod(2, 386);
    SetLod(3, 192);
    SetLod(4, 100);
    SetLod(5, 50);
    SetLod(6, 0);
    SetLod(7, 0);
}
// namespace GameEngine
int32 TerrainConfiguration::updateMorphingArea(uint32 lod)
{
    return static_cast<uint32>((perTerrainBuffer.scale.x / static_cast<float>(TerrainDef::TERRAIN_ROOT_NODES_COUNT)) /
                               static_cast<float>(pow(2, lod)));
}

void TerrainConfiguration::SetLod(uint32 index, uint32 value)
{
    if (index > LOD_SIZE)
    {
        return;
    }

    lodRanges_[index]     = value;

    switch (index)
    {
    case 0:
        perTerrainBuffer.morpharea1_4.x = value - updateMorphingArea(index + 1);
        break;
    case 1:
        perTerrainBuffer.morpharea1_4.y = value - updateMorphingArea(index + 1);
        break;
    case 2:
        perTerrainBuffer.morpharea1_4.z = value - updateMorphingArea(index + 1);
        break;
    case 3:
        perTerrainBuffer.morpharea1_4.w = value - updateMorphingArea(index + 1);
        break;
    case 4:
        perTerrainBuffer.morpharea5_8.x = value - updateMorphingArea(index + 1);
        break;
    case 5:
        perTerrainBuffer.morpharea5_8.y = value - updateMorphingArea(index + 1);
        break;
    case 6:
        perTerrainBuffer.morpharea5_8.z = value - updateMorphingArea(index + 1);
        break;
    case 7:
        perTerrainBuffer.morpharea5_8.w = value - updateMorphingArea(index + 1);
        break;
    default:
        break;
    }
}
}  // namespace GameEngine
