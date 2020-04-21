#pragma once
#include "GameEngine/Camera/Camera.h"

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
    void Move() override;

private:
    vec3 CalculateInputs();
    void CalculatePitch(float);
    void CalculateYaw(float);
    vec2 CalcualteMouseMove();

private:
    Input::InputManager& inputManager_;
    DisplayManager& displayManager_;
};
}  // namespace GameEngine
