#include "CircleConstantHeightBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleConstantHeightBrush::CircleConstantHeightBrush(PaintContext& context)
    : CircleHeightBrush(context)
{
}
bool CircleConstantHeightBrush::main(const vec2ui& paintedPoint)
{
    auto heightMap = getHeightMap();

    if (heightMap)
    {
        auto currentHeightOpt = heightMap->GetHeight(paintedPoint);

        if (currentHeightOpt)
        {
            auto heightFactor = paintContext_.currentTerrainPoint->terrainComponent.GetTerrainConfiguration().GetScale().y;
            auto newHeight    = glm::mix(*currentHeightOpt, paintContext_.strength / heightFactor, intensity_);
            return heightMap->SetHeight(paintedPoint, newHeight);
        }
    }
    return false;
}
}  // namespace GameEngine
