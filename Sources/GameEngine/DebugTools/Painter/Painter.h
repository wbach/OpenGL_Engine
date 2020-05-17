#pragma once
#include <Types.h>

#include <memory>
#include <optional>

#include "TerrainPointGetter.h"
#include "StepInterpolation.h"
#include "Brushes/HeightBrushType.h"
#include "Brushes/TextureBrushType.h"
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

class Painter
{
public:
    struct EntryParamters
    {
        Input::InputManager& inputManager;
        const CameraWrapper& camera;
        const Projection& projection;
        const vec2ui& windowSize;
        const Components::ComponentController& componentController;
    };
    Painter(const EntryParamters&, PaintType);
    Painter(const EntryParamters&, PaintType, float, int32);
    ~Painter();
    virtual void SetBrush(const std::string&) = 0;
    virtual std::string SelectedBrush() const = 0;
    virtual std::vector<std::string> AvaiableBrushTypes() const = 0;

    void Paint(const vec2&);
    std::optional<vec3> GetMouseTerrainPosition(const vec2&);
    PaintType GetPaintType() const;

public:
    float strength_;
    int32 brushSize_;

    StepInterpolation stepInterpolation_;

protected:
    virtual void Paint(const TerrainPoint&) = 0;

protected:
    Input::InputManager& inputManager_;
    TerrainPointGetter pointGetter_;
    std::optional<uint32> mouseKeyDownSubscribtion_;
    std::optional<uint32> mouseKeyUpSubscribtion_;
    bool lmouseKeyIsPressed_;

private:
    PaintType paintType_;
};
}  // namespace GameEngine

