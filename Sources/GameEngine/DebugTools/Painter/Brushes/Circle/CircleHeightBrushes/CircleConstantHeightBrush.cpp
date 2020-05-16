#include "CircleConstantHeightBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleConstantHeightBrush::CircleConstantHeightBrush(const TerrainPoint& terrainPoint, bool linearDistance,
                                                     const vec2& mousePosition, float strength, int32 brushSize)
    : CircleBrushBase(*terrainPoint.terrainComponent.GetHeightMap(), terrainPoint, linearDistance, mousePosition,
                      strength, brushSize)
    , heightMap_(*terrainPoint.terrainComponent.GetHeightMap())
{
}
bool CircleConstantHeightBrush::Main(const vec2ui& paintedPoint)
{
    auto currentHeightOpt = heightMap_.GetHeight(paintedPoint);

    if (currentHeightOpt)
    {
        auto heightFactor = terrainPoint_.terrainComponent.GetTerrainConfiguration().GetScale().y;
        auto newHeight = glm::mix(*currentHeightOpt, inputStrength_ / heightFactor, intensity_);
        return heightMap_.SetHeight(paintedPoint, newHeight);
    }
    return false;
}
}  // namespace GameEngine
