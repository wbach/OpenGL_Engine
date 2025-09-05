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
class ThirdPersonCamera : public Camera
{
public:
    ThirdPersonCamera(Input::InputManager&, const common::Transform&);
    ThirdPersonCamera(Input::InputManager&, const common::Transform&, const vec3&);
    ~ThirdPersonCamera() override;
    void Update() override;
    virtual bool ShouldMove();
    void CalculateMove();
    void CalculateZoom(float);
    void LockInputs(bool);
    void Lock() override;
    void Unlock() override;

protected:
    void SetRelativeMode(bool);
    vec2 CalcualteMouseMove();

protected:
    Input::InputManager& inputManager_;
    const common::Transform& lookAtTransform_;
    float distanceFromPlayer_;

    vec3 offset_;
    vec3 lookAtOffset_;
    float mouseSensitivity_;
    bool isRelativeModeEnabled_;
    bool lockInputs_;

    Utils::CClock clock_;
};
}  // namespace GameEngine
