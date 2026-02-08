#include "SlopeHeightPainter.h"

#include <Input/InputManager.h>
#include <Input/KeyCodes.h>
#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Painter/HeightPainter.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "IBrush.h"

namespace GameEngine
{
SlopeHeightPainter::SlopeHeightPainter(TerrainPainter::Dependencies&& dependencies, std::unique_ptr<IBrush> brush)
    : HeightPainter(std::move(dependencies), std::move(brush))
    , currentState(SlopeState::PlacingStart)
{
    keySubId = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [&]() { OnClick(); });
    LOG_DEBUG << "Select start point";
}

SlopeHeightPainter::~SlopeHeightPainter()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, keySubId);
}
void SlopeHeightPainter::Apply(Texture& texture, const vec2ui& paintedPoint, const Influance& influancePoint, DeltaTime deltaTime)
{
    try
    {
        if (currentState != SlopeState::Painting)
            return;

        auto& heightmap = dynamic_cast<HeightMap&>(texture);
        if (not IsInRange(heightmap.GetImage(), paintedPoint))
        {
            LOG_WARN << "Out of range";
            return;
        }
        vec2 A(startPoint->pixelPos.x, startPoint->pixelPos.y);
        vec2 B(endPoint->pixelPos.x, endPoint->pixelPos.y);
        vec2 P(paintedPoint.x, paintedPoint.y);

        float t       = glm::dot(P - A, direction) / length;
        float targetH = startPoint->height + t * (endPoint->height - startPoint->height);
        // float dist    = std::abs(direction.y * P.x - direction.x * P.y + B.x * A.y - B.y * A.x) / std::sqrt(length);

        heightmap.SetHeight(paintedPoint, targetH);
    }
    catch (const std::bad_cast& e)
    {
        std::cerr << "Bad cast: " << e.what() << std::endl;
    }
}
void SlopeHeightPainter::OnClick()
{
    if (currentState == SlopeState::Painting)
    {
        return;
    }

    auto currentTerrainPoint = pointGetter_.GetMousePointOnTerrain(inputManager_.GetMousePosition());
    if (not currentTerrainPoint)
    {
        LOG_DEBUG << "not currentTerrainPoint";
        return;
    }

    auto heightMap = currentTerrainPoint->terrainComponent->GetHeightMap();
    if (not heightMap)
    {
        LOG_DEBUG << "not heightMap";
        return;
    }

    vec2ui point = ConvertToPixelSpace(*currentTerrainPoint);
    auto height  = heightMap->GetHeight(point);

    if (not height)
    {
        LOG_DEBUG << "not height";
        return;
    }

    if (currentState == SlopeState::PlacingStart)
    {
        startPoint   = PointData{.pointOnTerrain = currentTerrainPoint->pointOnTerrain, .pixelPos = point, .height = *height};
        currentState = SlopeState::PlacingEnd;

        LOG_DEBUG << "Now select end point";
    }
    else if (currentState == SlopeState::PlacingEnd)
    {
        endPoint     = PointData{.pointOnTerrain = currentTerrainPoint->pointOnTerrain, .pixelPos = point, .height = *height};
        currentState = SlopeState::Painting;

        // direction = vec2(endPoint->pointOnTerrain.x, endPoint->pointOnTerrain.z) - vec2(startPoint->pointOnTerrain.x,
        // startPoint->pointOnTerrain.z);
        direction = vec2(endPoint->pixelPos.x, endPoint->pixelPos.y) - vec2(startPoint->pixelPos.x, startPoint->pixelPos.y);
        length    = glm::dot(direction, direction);

        LOG_DEBUG << "Start painting";
    }
}
}  // namespace GameEngine
