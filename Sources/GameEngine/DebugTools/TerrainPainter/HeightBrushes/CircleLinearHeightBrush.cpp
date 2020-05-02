#include "CircleLinearHeightBrush.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleLinearHeightBrush::CircleLinearHeightBrush(TerrainPoint& terrainPoint, bool linearDistance,
                                                 const vec2& mousePosition, float strength, int32 brushSize)
    : CircleBrushBase(terrainPoint, linearDistance, mousePosition, strength, brushSize)
{
}
bool CircleLinearHeightBrush::Main(HeightMap& heightMap, const vec2ui& paintedPoint, float strength)
{
    auto currentHeightOpt = heightMap.GetHeight(paintedPoint);
    if (currentHeightOpt)
    {
        auto currentHeight = *currentHeightOpt;
        auto newHeight     = currentHeight + strength;
        return heightMap.SetHeight(paintedPoint, newHeight);
    }
    return false;
}
}  // namespace GameEngine
