#include "CircleGaussianHeightBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleGaussianHeightBrush::CircleGaussianHeightBrush(PaintContext& context)
    : CircleHeightBrush(context)
    , imageFilter_(GraphicsApi::gaussian7x7Filter())
    , halfFilterSize_(imageFilter_.size.x / 2, imageFilter_.size.y / 2)
{
}
bool CircleGaussianHeightBrush::main(const vec2ui& paintedPoint)
{
    auto heightMap = getHeightMap();

    if (heightMap)
    {
        auto currentHeightOpt = heightMap->GetHeight(paintedPoint);
        if (not currentHeightOpt)
            return false;

        float newHeight = 0.f;

        for (uint32 fy = 0; fy < imageFilter_.size.y; ++fy)
        {
            for (uint32 fx = 0; fx < imageFilter_.size.x; ++fx)
            {
                uint32 x = paintedPoint.x + fx - halfFilterSize_.x;
                uint32 y = paintedPoint.y + fy - halfFilterSize_.y;

                vec2ui heightMapPos(x, y);
                auto neighborHeightOpt = heightMap->GetHeight(heightMapPos);

                if (neighborHeightOpt)
                {
                    auto weightOpt = imageFilter_.getSample(fx, fy);
                    if (weightOpt)
                    {
                        newHeight += (*neighborHeightOpt) * (*weightOpt);
                    }
                }
            }
        }

        if (not compare(*currentHeightOpt, newHeight))
        {
            heightMap->SetHeight(paintedPoint, newHeight);
            return true;
        }
    }

    return false;
}
}  // namespace GameEngine
