#include "CircleHeightBrush.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace
{
const vec2ui defaultTextureSize(0);
}
const vec2ui& CircleHeightBrush::getTextureSize() const
{
    const auto heightMap = getHeightMap();
    return heightMap ? heightMap->GetSize() : defaultTextureSize;
}
HeightMap* CircleHeightBrush::getHeightMap() const
{
    return paintContext_.currentTerrainPoint->terrainComponent.GetHeightMap();
}
}  // namespace GameEngine
