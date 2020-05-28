#include "CircleGaussianHeightBrush.h"

#include <Logger/Log.h>
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleGaussianHeightBrush::CircleGaussianHeightBrush(const TerrainPoint& terrainPoint, bool linearDistance,
                                                     float strength, int32 brushSize)
    : CircleBrushBase(*terrainPoint.terrainComponent.GetHeightMap(), terrainPoint, linearDistance, strength, brushSize)
    , heightMap_(*terrainPoint.terrainComponent.GetHeightMap())
    , imageFilter_(GraphicsApi::gaussian7x7Filter())
    , halfFilterSize_(imageFilter_.size.x / 2, imageFilter_.size.y / 2)
{
}
bool CircleGaussianHeightBrush::Main(const vec2ui& paintedPoint)
{
    auto currentHeightOpt = heightMap_.GetHeight(paintedPoint);
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
            auto neighborHeightOpt = heightMap_.GetHeight(heightMapPos);

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
        heightMap_.SetHeight(paintedPoint, newHeight);
        return true;
    }

    return false;
}
}  // namespace GameEngine
