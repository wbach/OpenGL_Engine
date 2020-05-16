#include "CircleAverageHeightBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleAverageHeightBrush::CircleAverageHeightBrush(const TerrainPoint& terrainPoint, bool linearDistance,
                                                   float strength, int32 brushSize)
    : CircleBrushBase(*terrainPoint.terrainComponent.GetHeightMap(), terrainPoint, linearDistance, strength, brushSize)
    , heightMap_(*terrainPoint.terrainComponent.GetHeightMap())
    , average_(0)
{
}
bool CircleAverageHeightBrush::Paint()
{
    CalcualteAverage();
    return CircleBrushBase::Paint();
}
bool CircleAverageHeightBrush::Main(const vec2ui& paintedPoint)
{
    auto currentHeightOpt = heightMap_.GetHeight(paintedPoint);

    if (currentHeightOpt)
    {
        auto currentHeight = *currentHeightOpt;
        auto distance      = fabsf(average_ - currentHeight);

        float newHeight = currentHeight;
        if (distance > std::numeric_limits<float>::epsilon())
        {
            if (average_ > newHeight)
            {
                newHeight += inputStrength_ * intensity_;
            }
            else
            {
                newHeight -= inputStrength_ * intensity_;
            }

            return heightMap_.SetHeight(paintedPoint, newHeight);
        }
    }
    return false;
}
void CircleAverageHeightBrush::CalcualteAverage()
{
    average_ = 0.f;
    float pointsCount{0.f};

    MainLoop([&](const auto& paintedPoint) {
        auto heightOpt = terrainPoint_.terrainComponent.GetHeightMap()->GetHeight(paintedPoint);
        if (heightOpt)
        {
            average_ += *heightOpt;
            ++pointsCount;
        }
    });

    average_ /= pointsCount;
}
}  // namespace GameEngine
