#include "TerrainPainter.h"

#include <Logger/Log.h>

#include <Input/InputManager.h>
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "HeightBrushes/CircleAverageHeightBrush.h"
#include "HeightBrushes/CircleConstantHeightBrush.h"
#include "HeightBrushes/CircleLinearHeightBrush.h"

namespace GameEngine
{
TerrainPainter::TerrainPainter(Input::InputManager& inputManager, const CameraWrapper& camera,
                               const Projection& projection, const vec2ui& windowSize,
                               const Components::ComponentController& componentController)
    : strength_(0.01f)
    , brushSize_(16)
    , heightBrushType_(HeightBrushType::CircleLinear)
    , stepInterpolation_(StepInterpolation::Linear)
    , inputManager_(inputManager)
    , pointGetter_(camera, projection, windowSize, componentController)
    , lmouseKeyIsPressed_(false)
{
    mouseKeyDownSubscribtion_ =
        inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() { lmouseKeyIsPressed_ = true; });

    mouseKeyUpSubscribtion_ =
            inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [this]() { lmouseKeyIsPressed_ = false; });
}

TerrainPainter::~TerrainPainter()
{
    if (mouseKeyDownSubscribtion_)
        inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, *mouseKeyDownSubscribtion_);

    if (mouseKeyUpSubscribtion_)
        inputManager_.UnsubscribeOnKeyUp(KeyCodes::LMOUSE, *mouseKeyUpSubscribtion_);
}

void TerrainPainter::PaintBlendMap(const vec2& mousePosition, const vec3&, float)
{
    if (not lmouseKeyIsPressed_)
        return;

    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (not terrainPoint)
        return;
}

#define PAINT(X)                                                                                                     \
    X(*terrainPoint, stepInterpolation_ == StepInterpolation::Linear, mousePosition, strength_, brushSize_) \
        .Paint()

void TerrainPainter::PaintHeightMap(const vec2& mousePosition)
{
    if (not lmouseKeyIsPressed_)
        return;

    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (not terrainPoint)
        return;

    auto heightMap = terrainPoint->terrainComponent.GetHeightMap();
    if (not heightMap)
    {
        return;
    }

    bool heightmapChange{false};

    switch (heightBrushType_)
    {
        case HeightBrushType::CircleLinear:
            heightmapChange = PAINT(CircleLinearHeightBrush);
            break;
        case HeightBrushType::CircleAverage:
            heightmapChange = PAINT(CircleAverageHeightBrush);
            break;
        case HeightBrushType::CircleConstantValue:
            heightmapChange = PAINT(CircleConstantHeightBrush);
            break;
    }

    if (heightmapChange)
        terrainPoint->terrainComponent.HeightMapChanged();
}
#undef PAINT

std::optional<vec3> TerrainPainter::GetMouseTerrainPosition(const vec2& mousePosition)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (terrainPoint)
        return terrainPoint->pointOnTerrain;

    return std::nullopt;
}
}  // namespace GameEngine
