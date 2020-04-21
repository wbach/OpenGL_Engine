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
    void Lock() override;
    void Unlock() override;

private:
    void CalculatePitch(float);
    void CalculateYaw(float);
    void LockPitch();
    void LockYaw();
    vec2 CalcualteMouseMove();

private:
    Input::InputManager& inputManager_;
    DisplayManager& displayManager_;

private:
    MeasurementValue& cameraPosiionDebug_;
    MeasurementValue& cameraRotationDebug_;
    MeasurementValue& moveVector_;
};
}  // namespace GameEngine
