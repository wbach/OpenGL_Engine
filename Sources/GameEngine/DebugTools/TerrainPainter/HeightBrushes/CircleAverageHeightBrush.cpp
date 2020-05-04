#include "CircleAverageHeightBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleAverageHeightBrush::CircleAverageHeightBrush(TerrainPoint& terrainPoint, bool linearDistance,
                                                   const vec2& mousePosition, float strength, int32 brushSize)
    : CircleBrushBase(terrainPoint, linearDistance, mousePosition, strength, brushSize)
    , average_(0)
{
}
bool CircleAverageHeightBrush::Main(HeightMap& heightMap, const vec2ui& paintedPoint, float strength)
{
    auto currentHeightOpt = heightMap.GetHeight(paintedPoint);

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

            return heightMap.SetHeight(paintedPoint, newHeight);
        }
    }
    return false;
}
void CircleAverageHeightBrush::OnStart()
{
    CalcualteAverage();
}
void CircleAverageHeightBrush::CalcualteAverage()
{
    average_ = 0.f;

    auto& heightMapImage = terrainPoint_.terrainComponent.GetHeightMap()->GetImage();

    float pointsCount{0.f};
    for (int32 y = -brushSize_; y < brushSize_; y++)
    {
        for (int32 x = -brushSize_; x < brushSize_; x++)
        {
            vec2i brushPoint = GetBrushPoint(x, y, terrainPoint_.terrainSpacePoint);
            if (not IsBrushInRange(brushPoint, vec2ui(heightMapImage.width, heightMapImage.height)))
                continue;

            if (((x) * (x) + (y) * (y)) <= brushSize_ * brushSize_)
            {
                vec2ui paintedPoint(brushPoint.x, brushPoint.y);
                auto heightOpt = terrainPoint_.terrainComponent.GetHeightMap()->GetHeight(paintedPoint);
                if (heightOpt)
                {
                    average_ += *heightOpt;
                    ++pointsCount;
                }
            }
        }
    }
    average_ /= pointsCount;
}
}  // namespace GameEngine
