#include "TerrainPainter.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "Brushes/Circle/CircleHeightBrushes/CircleAverageHeightBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleConstantHeightBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleLinearHeightBrush.h"
#include "Brushes/Circle/CircleTextureBrushes/CircleLinearTextureBrush.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Resources/Textures/MaterialTexture.h"

namespace GameEngine
{
TerrainPainter::TerrainPainter(Input::InputManager& inputManager, const CameraWrapper& camera,
                               const Projection& projection, const vec2ui& windowSize,
                               const Components::ComponentController& componentController)
    : paintType_(PaintType::HeightMap)
    , paintBlendMapColor_(1.f, 0.f, 0.f, 0.f)
    , strength_(0.01f)
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

void TerrainPainter::Paint(const vec2& mousePosition)
{
    switch (paintType_)
    {
        case PaintType::BlendMap:
            PaintBlendMap(mousePosition);
            break;
        case PaintType::HeightMap:
            PaintHeightMap(mousePosition);
            break;
    }
}

#define BRUSH(X) \
    X(*terrainPoint, stepInterpolation_ == StepInterpolation::Linear, mousePosition, strength_, brushSize_)

void TerrainPainter::PaintBlendMap(const vec2& mousePosition)
{
    if (not lmouseKeyIsPressed_)
        return;

    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (not terrainPoint)
        return;

    auto blendMapTexture = terrainPoint->terrainComponent.GetTexture(TerrainTextureType::blendMap);
    if (not blendMapTexture)
    {
        return;
    }

   // DEBUG_LOG("paintBlendMapColor_ " + std::to_string(paintBlendMapColor_.color));
    if (BRUSH(CircleLinearTextureBrush).SetColor(paintBlendMapColor_).Paint())
        terrainPoint->terrainComponent.BlendMapChanged();
}

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
            heightmapChange = BRUSH(CircleLinearHeightBrush).Paint();
            break;
        case HeightBrushType::CircleAverage:
            heightmapChange = BRUSH(CircleAverageHeightBrush).Paint();
            break;
        case HeightBrushType::CircleConstantValue:
            heightmapChange = BRUSH(CircleConstantHeightBrush).Paint();
            break;
    }

    if (heightmapChange)
        terrainPoint->terrainComponent.HeightMapChanged();
}
#undef BRUSH

std::optional<vec3> TerrainPainter::GetMouseTerrainPosition(const vec2& mousePosition)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (terrainPoint)
        return terrainPoint->pointOnTerrain;

    return std::nullopt;
}

void TerrainPainter::RecalculateNormals()
{
    for (auto& terrain : pointGetter_.GetSceneTerrains())
    {
        terrain->RecalculateNormals();
    }
}

void TerrainPainter::RecalcualteYOffset()
{
    for (auto& terrain : pointGetter_.GetSceneTerrains())
    {
        terrain->RecalculateYOffset();
    }
}
}  // namespace GameEngine
