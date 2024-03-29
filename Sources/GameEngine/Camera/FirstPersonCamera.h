#pragma once
#include "Camera.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class DisplayManager;

class FirstPersonCamera : public Camera
{
public:
    FirstPersonCamera(Input::InputManager&, DisplayManager&);
    void Update() override;
    void Lock() override;
    void MoveLock(bool);
    void Unlock() override;

private:
    vec3 CalculateInputs();
    void CalculatePitch(float);
    void CalculateYaw(float);
    vec2 CalcualteMouseMove();

private:
    Input::InputManager& inputManager_;
    DisplayManager& displayManager_;
    bool moveLock_;
};
}  // namespace GameEngine
