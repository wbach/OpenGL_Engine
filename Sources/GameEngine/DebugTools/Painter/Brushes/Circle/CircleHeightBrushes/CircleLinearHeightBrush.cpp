#include "CircleLinearHeightBrush.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/DebugTools/Painter/PaintContext.h"

namespace GameEngine
{
CircleLinearHeightBrush::CircleLinearHeightBrush(PaintContext& context)
    : CircleHeightBrush(context)
{
}
bool CircleLinearHeightBrush::main(const vec2ui& paintedPoint)
{
    auto heightMap = getHeightMap();

    if (heightMap)
    {
        auto currentHeightOpt = heightMap->GetHeight(paintedPoint);
        if (currentHeightOpt)
        {
            auto currentHeight = *currentHeightOpt;
            auto newHeight     = currentHeight + (paintContext_.strength * intensity_);
            return heightMap->SetHeight(paintedPoint, newHeight);
        }
    }
    return false;
}
}  // namespace GameEngine
