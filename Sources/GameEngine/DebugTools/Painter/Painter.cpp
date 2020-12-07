#include "Painter.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "Brushes/Circle/CircleHeightBrushes/CircleAverageHeightBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleConstantHeightBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleLinearHeightBrush.h"
#include "Brushes/Circle/CircleTextureBrushes/CircleLinearTextureBrush.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
Painter::Painter(const EntryParamters& entryParamters, PaintType paintType)
    : Painter(entryParamters, paintType, 0.01f, 16)
{
}

Painter::Painter(const EntryParamters& entryParamters, PaintType paintType, float strength, int32 brushSize)
    : paintContext_{strength, brushSize, StepInterpolation::Linear}
    , inputManager_(entryParamters.inputManager)
    , pointGetter_(entryParamters.camera, entryParamters.projection, entryParamters.windowSize,
                   entryParamters.componentController)
    , lmouseKeyIsPressed_(false)
    , paintType_(paintType)
{
    mouseKeyDownSubscribtion_ = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() {
        auto terrainPoint = pointGetter_.GetMousePointOnTerrain(inputManager_.GetMousePosition());
        if (terrainPoint)
        {
            paintContext_.startedTerrainPoint = std::make_unique<TerrainPoint>(*terrainPoint);
        }
        lmouseKeyIsPressed_ = true;
    });

    mouseKeyUpSubscribtion_ = inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [this]() {
        paintContext_.startedTerrainPoint.reset();
        paintContext_.currentTerrainPoint.reset();
        lmouseKeyIsPressed_ = false;
    });
}

Painter::~Painter()
{
    if (mouseKeyDownSubscribtion_)
        inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, *mouseKeyDownSubscribtion_);

    if (mouseKeyUpSubscribtion_)
        inputManager_.UnsubscribeOnKeyUp(KeyCodes::LMOUSE, *mouseKeyUpSubscribtion_);
}

void Painter::paint()
{
    if (lmouseKeyIsPressed_)
    {
        auto terrainPoint = pointGetter_.GetMousePointOnTerrain(inputManager_.GetMousePosition());

        if (terrainPoint)
        {
            paintContext_.currentTerrainPoint = std::make_unique<TerrainPoint>(*terrainPoint);
            paintImpl();
        }
    }
}

PaintType Painter::getPaintType() const
{
    return paintType_;
}
float Painter::strength() const
{
    return paintContext_.strength;
}
int32 Painter::brushSize() const
{
    return paintContext_.brushSize;
}
StepInterpolation Painter::stepInterpolation() const
{
    return paintContext_.stepInterpolation;
}
void Painter::strength(float v)
{
    paintContext_.strength = v;
}
void Painter::brushSize(int32 v)
{
    paintContext_.brushSize = v;
}
void Painter::stepInterpolation(StepInterpolation v)
{
    paintContext_.stepInterpolation = v;
}
}  // namespace GameEngine
