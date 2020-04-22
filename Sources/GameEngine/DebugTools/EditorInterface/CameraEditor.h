#pragma once
#include "GameEngine/Camera/Camera.h"
#include <optional>

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class DisplayManager;

class CameraEditor : public BaseCamera
{
public:
    CameraEditor(Input::InputManager&, DisplayManager&);
    ~CameraEditor();
    void Move() override;

private:
    vec3 CalculateInputs();
    void CalculatePitch(float);
    void CalculateYaw(float);
    vec2 CalcualteMouseMove();

private:
    Input::InputManager& inputManager_;
    DisplayManager& displayManager_;
    uint32 mouseKeyDownSubscribtion_;
    uint32 mouseKeyUpSubscribtion_;
    std::optional<vec2i> referenceMousePosition_;
};
}  // namespace GameEngine
