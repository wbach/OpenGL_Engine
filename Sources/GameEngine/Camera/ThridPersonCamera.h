#pragma once
#include "Camera.h"
#include "Clock.hpp"

namespace common
{
class Transform;
}  // namespace common

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class ThirdPersonCamera : public BaseCamera
{
public:
    ThirdPersonCamera(Input::InputManager&, const common::Transform&);
    ThirdPersonCamera(Input::InputManager&, const common::Transform&, const vec3&);
    ~ThirdPersonCamera() override;
    void Move() override;
    void CalculateInput() override;
    void CalculateZoom(float) override;
    void Lock() override;
    void Unlock() override;

private:
    void SetRelativeMode(bool);
    vec2 CalcualteMouseMove();

private:
    Input::InputManager& inputManager_;
    const common::Transform& lookAtTransform_;
    float angleAroundPlayer_;
    float distanceFromPlayer_;

    vec3 offset_;
    vec3 lookAtOffset_;
    float mouseSensitivity_;
    bool captureMouse_;
    bool isRelativeModeEnabled_;

    Utils::CClock clock_;
};
}  // namespace GameEngine
