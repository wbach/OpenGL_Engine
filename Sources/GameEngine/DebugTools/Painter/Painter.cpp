#include "Painter.h"

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
Painter::Painter(const EntryParamters& entryParamters, PaintType paintType)
    : strength_(0.01f)
    , brushSize_(16)
    , stepInterpolation_(StepInterpolation::Linear)
    , inputManager_(entryParamters.inputManager)
    , pointGetter_(entryParamters.camera, entryParamters.projection, entryParamters.windowSize, entryParamters.componentController)
    , lmouseKeyIsPressed_(false)
    , paintType_(paintType)
{
    mouseKeyDownSubscribtion_ =
        inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() { lmouseKeyIsPressed_ = true; });

    mouseKeyUpSubscribtion_ =
        inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [this]() { lmouseKeyIsPressed_ = false; });
}

Painter::~Painter()
{
    if (mouseKeyDownSubscribtion_)
        inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, *mouseKeyDownSubscribtion_);

    if (mouseKeyUpSubscribtion_)
        inputManager_.UnsubscribeOnKeyUp(KeyCodes::LMOUSE, *mouseKeyUpSubscribtion_);
}

void Painter::Paint(const vec2& mousePosition)
{
    if (not lmouseKeyIsPressed_)
        return;

    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (terrainPoint)
    {
        Paint(mousePosition, *terrainPoint);
    }
}

std::optional<vec3> Painter::GetMouseTerrainPosition(const vec2& mousePosition)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (terrainPoint)
        return terrainPoint->pointOnTerrain;

    return std::nullopt;
}
PaintType Painter::GetPaintType() const
{
    return paintType_;
}
}  // namespace GameEngine
