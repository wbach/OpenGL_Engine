#include "CircleConstantHeightBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleConstantHeightBrush::CircleConstantHeightBrush(TerrainPoint& terrainPoint, bool linearDistance,
                                                     const vec2& mousePosition, float strength, int32 brushSize)
    : CircleBrushBase(terrainPoint, linearDistance, mousePosition, strength, brushSize)
{
}
bool CircleConstantHeightBrush::Main(HeightMap& heightMap, const vec2ui& paintedPoint, float)
{
    auto currentHeightOpt = heightMap.GetHeight(paintedPoint);

    if (currentHeightOpt)
    {
        auto heightFactor = terrainPoint_.terrainComponent.GetTerrainConfiguration().GetScale().y;
        auto newHeight = glm::mix(*currentHeightOpt, inputStrength_ / heightFactor, intensity_);
        return heightMap.SetHeight(paintedPoint, newHeight);
    }
    return false;
}
}  // namespace GameEngine
