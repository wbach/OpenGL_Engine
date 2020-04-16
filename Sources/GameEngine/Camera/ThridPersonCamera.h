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
    void LockCamera();
    void LockPitch();
    void LockYaw();
    void CalculateCameraPosition(float cameraYaw, float horizontalDistance, float verticalDistance);
    float CalculateHorizontalDistance();
    float CalculateVerticalDistance();
    void CalculateYaw(float cameraYaw);
    vec2 CalcualteMouseMove();
    void CalculatePitch(const vec2& mouseMove);
    void CalculateAngleAroundPlayer(const vec2& mouseMove);

private:
    Input::InputManager& inputManager_;
    const common::Transform& lookAtTransform_;
    float angleAroundPlayer_;
    float distanceFromPlayer_;

    vec3 offset_;
    float mouseSensitivity_;
    bool captureMouse_;

    Utils::CClock clock_;
};
}  // namespace GameEngine
