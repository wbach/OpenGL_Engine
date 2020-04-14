#pragma once
#include <list>

#include "Camera.h"
#include "Clock.hpp"
#include "Mutex.hpp"

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
    ThirdPersonCamera(Input::InputManager&, common::Transform&);
    ~ThirdPersonCamera() override;
    void Move() override;
    void CalculateInput() override;
    void CalculateZoom(float) override;
    void Lock() override;
    void Unlock() override;

private:
    void LockCamera();
    void SetCaptureMouse(bool capture);
    void LockPitch();
    void LockYaw();
    void CalculateCameraPosition(float horizontalDistance, float verticalDistance);
    float CalculateHorizontalDistance();
    float CalculateVerticalDistance();
    void CalculateYaw();
    vec2 CalcualteMouseMove();
    void CalculatePitch(const vec2& mouseMove);
    void CalculateAngleAroundPlayer(const vec2& mouseMove);

private:
    Input::InputManager& inputManager_;
    common::Transform& lookAtTransform_;
    float angleAroundPlayer_;
    float distanceFromPlayer_;

    bool isShowCursor;
    vec3 offset;
    float mousevel;
    bool captureMouse;

    Utils::CClock clock;
};
}  // namespace GameEngine
