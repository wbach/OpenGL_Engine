#include "CircleBrushBase.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Painter/PaintContext.h"
#include "GameEngine/DebugTools/Painter/TerrainPoint.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
CircleBrushBase::CircleBrushBase(PaintContext& context)
    : paintContext_(context)
{
}
bool CircleBrushBase::paint()
{
    bool isChanged{false};
    mainLoop(
        [&](const auto& paintedPoint)
        {
            if (main(paintedPoint))
            {
                isChanged = true;
            }
        });
    return isChanged;
}
vec2ui CircleBrushBase::getBrushPoint(int32 x, int32 y)
{
    return getBrushPoint(*paintContext_.currentTerrainPoint, x, y);
}
vec2ui CircleBrushBase::getBrushPoint(const TerrainPoint& terrainPoint, int32 x, int32 y)
{
    const auto& terrainLocalPoint = terrainPoint.terrainSpacePoint;

    const auto& textureSize = getTextureSize();
    auto imageCoordX        = static_cast<int32>(terrainLocalPoint.x * static_cast<float>(textureSize.x));
    auto imageCoordY        = static_cast<int32>(terrainLocalPoint.y * static_cast<float>(textureSize.y));

    return vec2ui(static_cast<uint32>(imageCoordX + x), static_cast<uint32>(imageCoordY + y));
}
bool CircleBrushBase::isInRange(const vec2ui& brushPoint)
{
    return isInRange(getTextureSize(), brushPoint);
}
bool CircleBrushBase::isInRange(const vec2ui& textureSize, const vec2ui& brushPoint)
{
    return (brushPoint.x < textureSize.x) and (brushPoint.y < textureSize.y);
}
void CircleBrushBase::mainLoop(std::function<void(const vec2ui&)> action)
{
    auto brushSize = paintContext_.brushSize;

    for (int32 y = -brushSize; y < brushSize; y++)
    {
        for (int32 x = -brushSize; x < brushSize; x++)
        {
            auto paintedPoint = getBrushPoint(x, y);
            if (not isInRange(paintedPoint))
                continue;

            if (((x) * (x) + (y) * (y)) <= brushSize * brushSize)
            {
                calculateIntensity(paintedPoint);
                action(paintedPoint);
            }
        }
    }
}
void CircleBrushBase::calculateIntensity(const vec2ui& paintedPoint)
{
    if (paintContext_.stepInterpolation == StepInterpolation::Linear)
    {
        auto centerPoint = getBrushPoint(0, 0);
        vec2 centerPointf(centerPoint.x, centerPoint.y);

        auto currentPoint = vec2(paintedPoint.x, paintedPoint.y);
        float distance    = glm::length(currentPoint - centerPointf) / static_cast<float>(paintContext_.brushSize);
        intensity_        = 1.f - distance;

        if (intensity_ < 0)
            intensity_ = 0.f;
        else if (intensity_ > 1.f)
            intensity_ = 1.f;
    }
    else
    {
        intensity_ = 1.f;
    }
}
}  // namespace GameEngine
