#pragma once
#include <Types.h>
#include <optional>

#include "TerrainPointGetter.h"
#include "PaintType.h"
#include "PaintContext.h"

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
    virtual ~Painter();
    virtual void setBrush(const std::string&) = 0;
    virtual std::string selectedBrush() const = 0;
    virtual std::vector<std::string> avaiableBrushTypes() const = 0;
    virtual float getWorldScaleBrushSize() = 0;

    std::optional<vec3> paint();
    PaintType getPaintType() const;

    float strength() const;
    int32 brushSize() const;
    StepInterpolation stepInterpolation() const;
    void strength(float);
    void brushSize(int32);
    void stepInterpolation(StepInterpolation);

protected:
    virtual void paintImpl() = 0;
    virtual void calculateWorldScaleBrushSize() = 0;
    PaintContext paintContext_;

protected:
    const Components::ComponentController& componentController_;
    Input::InputManager& inputManager_;
    TerrainPointGetter pointGetter_;
    std::optional<uint32> mouseKeyDownSubscribtion_;
    std::optional<uint32> mouseKeyUpSubscribtion_;
    bool lmouseKeyIsPressed_;

private:
    PaintType paintType_;
};
}  // namespace GameEngine

