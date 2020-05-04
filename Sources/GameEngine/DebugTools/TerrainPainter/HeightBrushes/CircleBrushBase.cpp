#include "CircleBrushBase.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
CircleBrushBase::CircleBrushBase(TerrainPoint& terrainPoint, bool linearDistance, const vec2& mousePosition,
                                 float strength, int32 brushSize)
    : terrainPoint_(terrainPoint)
    , linearDistance_(linearDistance)
    , mousePosition_(mousePosition)
    , inputStrength_(strength)
    , brushSize_(brushSize)
{
}

bool CircleBrushBase::Paint()
{
    OnStart();

    bool isHeightMapChanged{false};
    auto& heightMapImage = terrainPoint_.terrainComponent.GetHeightMap()->GetImage();

    for (int32 y = -brushSize_; y < brushSize_; y++)
    {
        for (int32 x = -brushSize_; x < brushSize_; x++)
        {
            vec2i brushPoint = GetBrushPoint(x, y, terrainPoint_.terrainSpacePoint);
            if (not IsBrushInRange(brushPoint, vec2ui(heightMapImage.width, heightMapImage.height)))
                continue;

            vec2ui paintedPoint(brushPoint.x, brushPoint.y);

            if (((x) * (x) + (y) * (y)) <= brushSize_ * brushSize_)
            {
                if (Main(*terrainPoint_.terrainComponent.GetHeightMap(), paintedPoint,
                         CalculateStrength(paintedPoint)))
                {
                    isHeightMapChanged = true;
                }
            }
        }
    }

    return isHeightMapChanged;
}
vec2i CircleBrushBase::GetBrushPoint(int32 x, int32 y, const vec2ui& terrainPoint)
{
    int32 mx = static_cast<int32>(terrainPoint.x) + x;
    int32 my = static_cast<int32>(terrainPoint.y) + y;
    return vec2i(mx, my);
}
bool CircleBrushBase::IsBrushInRange(const vec2i& brushPoint, const vec2ui& size)
{
    return (brushPoint.x >= 0) and (brushPoint.x < static_cast<int32>(size.x)) and (brushPoint.y >= 0) and
           (brushPoint.y < static_cast<int32>(size.y));
}
void CircleBrushBase::OnStart()
{
}
float CircleBrushBase::CalculateStrength(const vec2ui& paintedPoint)
{
    intensity_ = 1.f;

    if (linearDistance_)
    {
        auto currentPoint      = vec2(paintedPoint.x, paintedPoint.y);
        auto pointOnHeightMapf = vec2(terrainPoint_.terrainSpacePoint.x, terrainPoint_.terrainSpacePoint.y);
        float distance         = glm::length(currentPoint - pointOnHeightMapf) / static_cast<float>(brushSize_);
        intensity_ = 1.f - distance;

        if (intensity_ < 0)
            intensity_ = 0.f;
    }

    return intensity_ * inputStrength_;
}
}  // namespace GameEngine
