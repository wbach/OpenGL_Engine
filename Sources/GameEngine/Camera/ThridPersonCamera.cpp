#include "ThridPersonCamera.h"

#include <Common/Transform.h>
#include <GLM/GLMUtils.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils.h>

namespace GameEngine
{
ThirdPersonCamera::ThirdPersonCamera(Input::InputManager& inputManager, const common::Transform& lookAt)
    : ThirdPersonCamera(inputManager, lookAt, vec3(0))
{
}
ThirdPersonCamera::ThirdPersonCamera(Input::InputManager& inputManager, const common::Transform& lookAt,
                                     const vec3& lookAtOffset)
    : inputManager_(inputManager)
    , lookAtTransform_(lookAt)
    , distanceFromPlayer_(3.f)
    , offset_(0, 0, 1.f)
    , lookAtOffset_(lookAtOffset)
    , mouseSensitivity_(.4f)
    , isRelativeModeEnabled_(false)
    , lockInputs_{false}
    , clock_(std::chrono::milliseconds(5))
{
    SetRelativeMode(true);
}
ThirdPersonCamera::~ThirdPersonCamera()
{
}
void ThirdPersonCamera::CalculateInput()
{
    if (lockInputs_)
        return;

     if (inputManager_.GetKey(KeyCodes::LCTRL) or lock_)
    {
        inputManager_.ShowCursor(true);
        SetRelativeMode(false);
        return;
    }
     SetRelativeMode(true);
     inputManager_.ShowCursor(false);

    if (!clock_.OnTick())
        return;

    vec2 move  = CalcualteMouseMove() * mouseSensitivity_;

    auto pitch = glm::angleAxis(glm::radians(move.y), vec3(1, 0, 0));
    auto yaw   = glm::angleAxis(glm::radians(move.x), vec3(0, 1, 0));
    offset_    = pitch * yaw * offset_;
}

void ThirdPersonCamera::Update()
{
    CalculateInput();

    auto lookAtPosition = lookAtTransform_.GetPosition() + lookAtOffset_;
    SetPosition(lookAtPosition + (lookAtTransform_.GetRotation().value_ * offset_ * distanceFromPlayer_));
    LookAt(lookAtPosition);

}
void ThirdPersonCamera::CalculateZoom(float v)
{
    distanceFromPlayer_ += v;
}
void ThirdPersonCamera::LockInputs(bool state)
{
    lockInputs_ = state;
}
void ThirdPersonCamera::Lock()
{
    Camera::Lock();
    SetRelativeMode(false);
    inputManager_.ShowCursor(true);
}
void ThirdPersonCamera::Unlock()
{
    Camera::Unlock();
    SetRelativeMode(true);
    inputManager_.ShowCursor(false);
}

void ThirdPersonCamera::SetRelativeMode(bool v)
{
    if (v != isRelativeModeEnabled_)
    {
        inputManager_.SetReleativeMouseMode(v);
        isRelativeModeEnabled_ = v;
    }
}
vec2 ThirdPersonCamera::CalcualteMouseMove()
{
    auto v = inputManager_.CalcualteMouseMove();
    return vec2(v.x, v.y);
}
}  // namespace GameEngine
