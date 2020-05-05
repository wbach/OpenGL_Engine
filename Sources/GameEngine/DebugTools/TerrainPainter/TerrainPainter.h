#pragma once
#include <Types.h>

#include <memory>
#include <optional>

#include "TerrainPointGetter.h"
#include "StepInterpolation.h"
#include "Brushes/HeightBrushType.h"
#include "PaintType.h"

namespace Input
{
class InputManager;
} // namespace Input

namespace common
{
class Transform;
}  // namespace common

namespace GameEngine
{
class GameObject;
class HeightMap;

namespace Components
{
class ComponentController;
}  // namespace Components

class TerrainPainter
{
public:
    TerrainPainter(Input::InputManager&, const CameraWrapper&, const Projection&, const vec2ui&, const Components::ComponentController&);
    ~TerrainPainter();
    void Paint(const vec2&);
    std::optional<vec3> GetMouseTerrainPosition(const vec2&);
    void RecalculateNormals();
    void RecalcualteYOffset();

private:
    void PaintBlendMap(const vec2&);
    void PaintHeightMap(const vec2&);

public:
    PaintType paintType_;
    Color paintBlendMapColor_;
    float strength_;
    int32 brushSize_;
    HeightBrushType heightBrushType_;
    StepInterpolation stepInterpolation_;

private:
    Input::InputManager& inputManager_;
    TerrainPointGetter pointGetter_;
    std::optional<uint32> mouseKeyDownSubscribtion_;
    std::optional<uint32> mouseKeyUpSubscribtion_;
    bool lmouseKeyIsPressed_;
};
}  // namespace GameEngine

