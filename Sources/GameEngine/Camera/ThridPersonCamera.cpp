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
                                     const vec3& offset)
    : inputManager_(inputManager)
    , lookAtTransform_(lookAt)
    , angleAroundPlayer_(0.f)
    , distanceFromPlayer_(3.f)
    , offset_(offset)
    , mouseSensitivity_(.4f)
    , clock_(std::chrono::milliseconds(5))
{
    inputManager.SetReleativeMouseMode(true);
}
ThirdPersonCamera::~ThirdPersonCamera()
{
}
void ThirdPersonCamera::LockPitch()
{
    if (GetRotation().x > 90.f)
        SetPitch(90.f);
    if (GetRotation().x < -90.f)
        SetPitch(-90.f);
}
void ThirdPersonCamera::LockYaw()
{
    if (GetRotation().y < 0.f)
        IncreaseYaw(360.f);
    if (GetRotation().y > 360.f)
        IncreaseYaw(-360.f);
}
void ThirdPersonCamera::LockCamera()
{
    LockPitch();
    LockYaw();
}
void ThirdPersonCamera::CalculateInput()
{
    if (inputManager_.GetKey(KeyCodes::LCTRL) or lock_)
    {
        inputManager_.ShowCursor(true);
        return;
    }

    inputManager_.ShowCursor(false);

    if (!clock_.OnTick())
        return;

    vec2 move = CalcualteMouseMove() * mouseSensitivity_;
    CalculatePitch(move);
    CalculateAngleAroundPlayer(move);
}

void ThirdPersonCamera::Move()
{
    if (lock_)
        return;

    float horizontalDistance = CalculateHorizontalDistance();
    float verticalDistance   = CalculateVerticalDistance();
    CalculateCameraPosition(horizontalDistance, verticalDistance);
    CalculateYaw();
    LockCamera();
}
void ThirdPersonCamera::CalculateCameraPosition(float horizontalDistance, float verticalDistance)
{
    float theata  = lookAtTransform_.GetRotation().GetEulerDegrees()->y + angleAroundPlayer_;
    float xOffset = (float)(horizontalDistance * sin(glm::radians(theata)));
    float zOffset = (float)(horizontalDistance * cos(glm::radians(theata)));

    vec3 pos;
    pos.x = lookAtTransform_.GetPosition().x - xOffset;
    pos.y = lookAtTransform_.GetPosition().y + verticalDistance;
    pos.z = lookAtTransform_.GetPosition().z - zOffset;
    pos += offset_;

    SetPosition(pos);
}
float ThirdPersonCamera::CalculateHorizontalDistance()
{
    return distanceFromPlayer_ * cosf(glm::radians(GetRotation().x));
}
float ThirdPersonCamera::CalculateVerticalDistance()
{
    return distanceFromPlayer_ * sinf(glm::radians(GetRotation().x));
}
void ThirdPersonCamera::CalculateYaw()
{
    SetYaw(180.f - (lookAtTransform_.GetRotation().GetEulerDegrees()->y + angleAroundPlayer_));
}
void ThirdPersonCamera::CalculateZoom(float v)
{
    distanceFromPlayer_ += v;
}
void ThirdPersonCamera::Lock()
{
    BaseCamera::Lock();
    inputManager_.SetReleativeMouseMode(false);
}
void ThirdPersonCamera::Unlock()
{
    BaseCamera::Unlock();
    inputManager_.SetReleativeMouseMode(true);
}
vec2 ThirdPersonCamera::CalcualteMouseMove()
{
    auto v = inputManager_.CalcualteMouseMove();
    return vec2(v.x, v.y);
}
void ThirdPersonCamera::CalculatePitch(const vec2& mouseMove)
{
    SetPitch(GetPitch() + mouseMove.y);
}

void ThirdPersonCamera::CalculateAngleAroundPlayer(const vec2& mouseMove)
{
    angleAroundPlayer_ -= mouseMove.x;
}
}  // namespace GameEngine
