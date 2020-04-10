#include "TerrainUtils.h"
#include "Resources/Textures/HeightMap.h"
#include "Components/Renderer/Terrain/TerrainConfiguration.h"

namespace GameEngine
{
float GetTerrainHeight(const std::vector<float>& data, float heightFactor, uint32 heightMapWidth, float offset, uint32 x, uint32 y)
{
    return (data[x + y * heightMapWidth] * heightFactor) - offset;
}
}  // namespace GameEngine
