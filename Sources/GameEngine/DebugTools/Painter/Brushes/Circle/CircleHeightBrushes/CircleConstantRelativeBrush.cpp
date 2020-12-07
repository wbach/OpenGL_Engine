#include "CircleConstantRelativeBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleConstantRelative::CircleConstantRelative(PaintContext& context)
    : CircleHeightBrush(context)
{
}
bool CircleConstantRelative::main(const vec2ui& paintedPoint)
{
    if (not paintContext_.startedTerrainPoint)
        return false;

    auto heightMap             = getHeightMap();
    auto startedPointHeightMap = paintContext_.startedTerrainPoint->terrainComponent.GetHeightMap();

    if (heightMap and startedPointHeightMap)
    {
        auto brushPoint = getBrushPoint(*paintContext_.startedTerrainPoint);

        if (isInRange(startedPointHeightMap->GetSize(), brushPoint))
        {
            auto heightUnderCreation = startedPointHeightMap->GetHeight(brushPoint);
            auto currentHeightOpt    = heightMap->GetHeight(paintedPoint);

            if (currentHeightOpt and heightUnderCreation)
            {
                auto newHeight = glm::mix(*currentHeightOpt, (*heightUnderCreation), intensity_);
                return heightMap->SetHeight(paintedPoint, newHeight);
            }
        }
    }
    return false;
}
}  // namespace GameEngine
