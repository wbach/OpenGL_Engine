#include "CircleBrushBase.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
CircleBrushBase::CircleBrushBase(Texture& texture, const TerrainPoint& terrainPoint, bool linearDistance,
                                 float strength, int32 brushSize)
    : terrainPoint_(terrainPoint)
    , texture_(texture)
    , linearDistance_(linearDistance)
    , inputStrength_(strength)
    , brushSize_(brushSize)
{
}
bool CircleBrushBase::Paint()
{
    bool isChanged{false};
    MainLoop([&](const auto& paintedPoint) {
        if (Main(paintedPoint))
        {
            isChanged = true;
        }
    });
    return isChanged;
}
vec2ui CircleBrushBase::GetBrushPoint(int32 x, int32 y)
{
    const auto& terrainLocalPoint = terrainPoint_.terrainSpacePoint;

    auto imageCoordX = static_cast<int32>(terrainLocalPoint.x * static_cast<float>(texture_.GetSize().x));
    auto imageCoordY = static_cast<int32>(terrainLocalPoint.y * static_cast<float>(texture_.GetSize().y));

    return vec2ui(static_cast<uint32>(imageCoordX + x), static_cast<uint32>(imageCoordY + y));
}
bool CircleBrushBase::IsInRange(const vec2ui& brushPoint)
{
    return (brushPoint.x < texture_.GetSize().x) and (brushPoint.y < texture_.GetSize().y);
}
void CircleBrushBase::MainLoop(std::function<void(const vec2ui&)> action)
{
    for (int32 y = -brushSize_; y < brushSize_; y++)
    {
        for (int32 x = -brushSize_; x < brushSize_; x++)
        {
            auto paintedPoint = GetBrushPoint(x, y);
            if (not IsInRange(paintedPoint))
                continue;

            if (((x) * (x) + (y) * (y)) <= brushSize_ * brushSize_)
            {
                CalculateIntensity(paintedPoint);
                action(paintedPoint);
            }
        }
    }
}
void CircleBrushBase::CalculateIntensity(const vec2ui& paintedPoint)
{
    if (linearDistance_)
    {
        auto centerPoint = GetBrushPoint(0, 0);
        vec2 centerPointf(centerPoint.x, centerPoint.y);

        auto currentPoint = vec2(paintedPoint.x, paintedPoint.y);
        float distance    = glm::length(currentPoint - centerPointf) / static_cast<float>(brushSize_);
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
