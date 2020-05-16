#include "CircleLinearHeightBrush.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleLinearHeightBrush::CircleLinearHeightBrush(const TerrainPoint& terrainPoint, bool linearDistance, float strength,
                                                 int32 brushSize)
    : CircleBrushBase(*terrainPoint.terrainComponent.GetHeightMap(), terrainPoint, linearDistance, strength, brushSize)
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
