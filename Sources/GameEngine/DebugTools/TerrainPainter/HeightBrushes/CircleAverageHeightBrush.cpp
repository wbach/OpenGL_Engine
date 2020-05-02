#include "CircleAverageHeightBrush.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include <Logger/Log.h>

namespace GameEngine
{
CircleAverageHeightBrush::CircleAverageHeightBrush(TerrainPoint& terrainPoint)
    : terrainPoint_(terrainPoint)
    , average_(0)
{
}
bool CircleAverageHeightBrush::Paint(const vec2& mousePosition, float strength, int32 brushSize)
{
    CalcualteAverage(brushSize);

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
                     brushSize, fabsf(strength), x, y))
            {
                isHeightMapChanged = true;
            }
        }
    }

    return isHeightMapChanged;
}
vec2i CircleAverageHeightBrush::GetBrushPoint(int32 x, int32 y, const vec2ui& terrainPoint)
{
    int32 mx = static_cast<int32>(terrainPoint.x) + x;
    int32 my = static_cast<int32>(terrainPoint.y) + y;
    return vec2i(mx, my);
}
bool CircleAverageHeightBrush::IsBrushInRange(const vec2i& brushPoint, const vec2ui& size)
{
    return (brushPoint.x >= 0) and (brushPoint.x < size.x) and (brushPoint.y >= 0) and (brushPoint.y < size.y);
}
bool CircleAverageHeightBrush::Main(HeightMap& heightMap, const vec2ui& pointOnHeightMap, const vec2ui& paintedPoint,
                                    int32 brushSize, float strength, int32 x, int32 y)
{
    bool isHeightMapChanged{false};
    if (((x) * (x) + (y) * (y)) <= brushSize * brushSize)
    {
        auto currentHeightOpt = heightMap.GetHeight(paintedPoint);
        auto currentPoint = vec2(paintedPoint.x, paintedPoint.y);
        auto pointOnHeightMapf = vec2(pointOnHeightMap.x, pointOnHeightMap.y);
        float distance = glm::length(currentPoint - pointOnHeightMapf) / static_cast<float>(brushSize);
        strength *= 1.f - distance;

        if (currentHeightOpt)
        {
            auto currentHeight = *currentHeightOpt;
            auto distance      = fabsf(average_ - currentHeight);

            float newHeight = currentHeight;
            if (distance > std::numeric_limits<float>::epsilon())
            {
                if (average_ > newHeight)
                {
                    newHeight += strength;
                }
                else
                {
                    newHeight -= strength;
                }

                auto succes = heightMap.SetHeight(paintedPoint, newHeight);

                if (succes)
                {
                    isHeightMapChanged = true;
                }
            }
        }
    }
    return isHeightMapChanged;
}
void CircleAverageHeightBrush::CalcualteAverage(int32 brushSize)
{
    average_ = 0.f;

    auto& heightMapImage = terrainPoint_.terrainComponent.GetHeightMap()->GetImage();

    float pointsCount{0.f};
    for (int32 y = -brushSize; y < brushSize; y++)
    {
        for (int32 x = -brushSize; x < brushSize; x++)
        {
            vec2i brushPoint = GetBrushPoint(x, y, terrainPoint_.pointOnHeightMap);
            if (not IsBrushInRange(brushPoint, vec2ui(heightMapImage.width, heightMapImage.height)))
                continue;

            vec2ui paintedPoint(brushPoint.x, brushPoint.y);
            auto heightOpt = terrainPoint_.terrainComponent.GetHeightMap()->GetHeight(paintedPoint);
            if (heightOpt)
            {
                average_ += *heightOpt;
                ++pointsCount;
            }
        }
    }

    average_ /= pointsCount;
    DEBUG_LOG("average_ " + std::to_string(average_));
}
}  // namespace GameEngine
