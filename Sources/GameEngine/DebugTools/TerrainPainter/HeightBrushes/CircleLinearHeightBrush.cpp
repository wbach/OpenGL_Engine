#include "CircleLinearHeightBrush.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleLinearHeightBrush::CircleLinearHeightBrush(TerrainPoint& terrainPoint)
    : terrainPoint_(terrainPoint)
{
}
bool CircleLinearHeightBrush::Paint(const vec2& mousePosition, float strength, int32 brushSize)
{
    bool isHeightMapChanged{false};
    auto& heightMapImage = terrainPoint_.terrainComponent.GetHeightMap()->GetImage();

    for (int32 y = -brushSize; y < brushSize; y++)
    {
        for (int32 x = -brushSize; x < brushSize; x++)
        {
            vec2i brushPoint = GetBrushPoint(x, y, terrainPoint_.pointOnHeightMap);
            if (not IsBrushInRange(brushPoint, vec2ui(heightMapImage.width, heightMapImage.height)))
                continue;

            vec2ui paintedPoint(brushPoint.x, brushPoint.y);

            if (Main(*terrainPoint_.terrainComponent.GetHeightMap(), terrainPoint_.pointOnHeightMap, paintedPoint,
                     brushSize, strength, x, y))
            {
                isHeightMapChanged = true;
            }
        }
    }

    return isHeightMapChanged;
}
vec2i CircleLinearHeightBrush::GetBrushPoint(int32 x, int32 y, const vec2ui& terrainPoint)
{
    int32 mx = static_cast<int32>(terrainPoint.x) + x;
    int32 my = static_cast<int32>(terrainPoint.y) + y;
    return vec2i(mx, my);
}
bool CircleLinearHeightBrush::IsBrushInRange(const vec2i& brushPoint, const vec2ui& size)
{
    return (brushPoint.x >= 0) and (brushPoint.x < size.x) and (brushPoint.y >= 0) and (brushPoint.y < size.y);
}
bool CircleLinearHeightBrush::Main(HeightMap& heightMap, const vec2ui& pointOnHeightMap, const vec2ui& paintedPoint,
                                   int32 brushSize, float strength, int32 x, int32 y)
{
    bool isHeightMapChanged{false};
    if (((x) * (x) + (y) * (y)) <= brushSize * brushSize)
    {
        auto currentPoint      = vec2(paintedPoint.x, paintedPoint.y);
        auto pointOnHeightMapf = vec2(pointOnHeightMap.x, pointOnHeightMap.y);
        float distance         = glm::length(currentPoint - pointOnHeightMapf) / static_cast<float>(brushSize);
        float r                = 1.f - distance;

        auto currentHeightOpt = heightMap.GetHeight(paintedPoint);

        if (currentHeightOpt)
        {
            auto currentHeight = *currentHeightOpt;
            auto newHeight     = currentHeight + (strength * r);
            auto succes        = heightMap.SetHeight(paintedPoint, newHeight);

            if (succes)
            {
                isHeightMapChanged = true;
            }
        }
    }
    return isHeightMapChanged;
}
}  // namespace GameEngine
