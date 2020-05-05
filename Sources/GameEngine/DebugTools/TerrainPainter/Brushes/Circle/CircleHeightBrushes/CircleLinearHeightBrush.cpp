#include "CircleLinearHeightBrush.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleLinearHeightBrush::CircleLinearHeightBrush(TerrainPoint& terrainPoint, bool linearDistance,
                                                 const vec2& mousePosition, float strength, int32 brushSize)
    : CircleBrushBase(*terrainPoint.terrainComponent.GetHeightMap(), terrainPoint, linearDistance, mousePosition,
                      strength, brushSize)
    , heightMap_(*terrainPoint.terrainComponent.GetHeightMap())
{
}
bool CircleLinearHeightBrush::Main(const vec2ui& paintedPoint)
{
    auto currentHeightOpt = heightMap_.GetHeight(paintedPoint);
    if (currentHeightOpt)
    {
        auto currentHeight = *currentHeightOpt;
        auto newHeight     = currentHeight + (inputStrength_ * intensity_);
        return heightMap_.SetHeight(paintedPoint, newHeight);
    }
    return false;
}
}  // namespace GameEngine
