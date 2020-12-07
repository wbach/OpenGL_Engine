#include "CircleAverageHeightBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleAverageHeightBrush::CircleAverageHeightBrush(PaintContext& context)
    : CircleHeightBrush(context)
    , average_(0)
{
}
bool CircleAverageHeightBrush::paint()
{
    calcualteAverage();
    return CircleBrushBase::paint();
}
bool CircleAverageHeightBrush::main(const vec2ui& paintedPoint)
{
    const auto heightMap = getHeightMap();

    if (heightMap)
    {
        auto currentHeightOpt = heightMap->GetHeight(paintedPoint);

        if (currentHeightOpt)
        {
            auto currentHeight = *currentHeightOpt;
            auto distance      = fabsf(average_ - currentHeight);

            float newHeight = currentHeight;
            if (distance > std::numeric_limits<float>::epsilon())
            {
                if (average_ > newHeight)
                {
                    newHeight += paintContext_.strength * intensity_;
                }
                else
                {
                    newHeight -= paintContext_.strength * intensity_;
                }

                return heightMap->SetHeight(paintedPoint, newHeight);
            }
        }
    }
    return false;
}
void CircleAverageHeightBrush::calcualteAverage()
{
    average_ = 0.f;
    float pointsCount{0.f};

    mainLoop([&](const auto& paintedPoint) {
        const auto heightMap = getHeightMap();
        if (heightMap)
        {
            auto heightOpt = heightMap->GetHeight(paintedPoint);
            if (heightOpt)
            {
                average_ += *heightOpt;
                ++pointsCount;
            }
        }
    });

    average_ /= pointsCount;
}
}  // namespace GameEngine
