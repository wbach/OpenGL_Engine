#pragma once
#include "Camera.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class DisplayManager;

class FirstPersonCamera : public BaseCamera
{
public:
    FirstPersonCamera(Input::InputManager&, DisplayManager&);
    void Move() override;
    void Lock() override;
    void Unlock() override;

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
