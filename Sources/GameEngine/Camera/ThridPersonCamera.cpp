#include "ThridPersonCamera.h"
#include <algorithm>
#include <cmath>
#include "Common/Transform.h"
#include "GLM/GLMUtils.h"
#include "Input/InputManager.h"
#include "Logger/Log.h"
#include "Utils.h"

#define DurationToFloatMs(x) std::chrono::duration<float, std::milli>(x).count()

namespace GameEngine
{
ThirdPersonCamera::ThirdPersonCamera(Input::InputManager& input_manager, common::Transform& look_at)
    : inputManager(input_manager)
    , lookAt_(look_at)
    , isShowCursor(false)
    , offset(0.f, 1.8f, 0.f)
    , mousevel(0.5f)
    , captureMouse(true)
    , destinationYaw(0)
    , destinationPitch(0)
    , moveTime(.125f)
    , destinationPosition(0)
    , referenceTime(std::chrono::high_resolution_clock::now())
    , distanceFromPlayer(2.f)
    , clock(std::chrono::milliseconds(5))
{
    inputManager.SetReleativeMouseMode(true);
}
ThirdPersonCamera::~ThirdPersonCamera()
{
    inputManager.ShowCursor(true);
}
void ThirdPersonCamera::SetCaptureMouse(bool capture)
{
    captureMouse = capture;
}
void ThirdPersonCamera::LockPitch()
{
    if (destinationPitch > 90.0f)
        destinationPitch = (90.0f);
    if (destinationPitch < -90.0f)
        destinationPitch = (-90.0f);
}
void ThirdPersonCamera::LockYaw()
{
    if (destinationYaw < 0.0f)
        destinationYaw += 360.0f;
    if (destinationYaw > 360.0f)
        destinationYaw -= 360.0f;
}
void ThirdPersonCamera::StaticCameraMove()
{
    SetPitch(destinationPitch);
    SetYaw(destinationYaw);
    SetPosition(destinationPosition);
}
void ThirdPersonCamera::LockCamera()
{
    LockPitch();
    LockYaw();
}
void ThirdPersonCamera::CalculateInput()
{
    if (inputManager.GetKey(KeyCodes::LCTRL) or lock_)
    {
        inputManager.ShowCursor(true);
        return;
    }

    inputManager.ShowCursor(false);

    if (!clock.OnTick())
        return;

    vec2 d_move = CalcualteMouseMove() * mousevel;
    CalculatePitch(d_move);
    CalculateAngleAroundPlayer(d_move);
}
float ThirdPersonCamera::GetTime() const
{
    auto currnet = std::chrono::high_resolution_clock::now() - referenceTime;
    return DurationToFloatMs(currnet) / 1000.0f;
}
bool ThirdPersonCamera::FindState(CameraState state)
{
    return std::find(states_.begin(), states_.end(), state) != states_.end();
}
template <class T>
T ThirdPersonCamera::ProcessState(CameraEvent<T>& stateInfo, const T& destination, float time, bool& remove)
{
    auto newValue = CalculateNewValueInTimeInterval<T>(stateInfo, time);

    float dt             = (stateInfo.endTime - time) / moveTime;
    stateInfo.startValue = newValue;
    stateInfo.moveValue  = destination - stateInfo.startValue;
    stateInfo.moveValue *= dt;
    stateInfo.startTime = time;

    if (time > stateInfo.endTime)
        remove = true;
    else
        remove = false;

    return newValue;
}
template <class T>
void ThirdPersonCamera::ControlState(CameraEvent<T>& stateInfo, CameraState state, const T& startValue,
                                     const T& destination, float time, bool use)
{
    if (!use)
    {
        if (!FindState(state))
        {
            states_.push_back(state);
            stateInfo.startValue = startValue;
            stateInfo.moveValue  = destination - stateInfo.startValue;
            stateInfo.startTime  = GetTime();
            stateInfo.endTime    = stateInfo.startTime + moveTime;
        }
    }
    else
    {
        if (FindState(state))
        {
            if (time > stateInfo.endTime)
            {
                states_.remove(state);
            }
        }
    }
}
void ThirdPersonCamera::Move()
{
    if (lock_) return;

    lookAtPosition_  = lookAt_.GetSnapShoot().position;
    lookAtRotataion_ = lookAt_.GetSnapShoot().rotation.GetEulerDegrees().value;

    float horizontal_distance = CalculateHorizontalDistance();
    float vertical_distance   = CalculateVerticalDistance();
    CalculateCameraPosition(horizontal_distance, vertical_distance);
    CalculateYaw();

    StaticCameraMove();
    // SmoothCameraMove();

    LockCamera();
}
void ThirdPersonCamera::SmoothCameraMove()
{
    float time = GetTime();

    for (auto state = states_.begin(); state != states_.end();)
    {
        bool remove = false;

        switch (*state)
        {
            case CameraState::MOVING:
            {
                auto newPos = ProcessState<vec3>(moveStateInfo_, destinationPosition, time, remove);
                SetPosition(newPos);
                break;
            }
            case CameraState::ROTATE_PITCH:
                break;
            case CameraState::ROTATE_YAW:
                break;
            case CameraState::SHAKING:
                break;
        }

        if (remove)
        {
            state = states_.erase(state);
        }
        else
        {
            ++state;
        }
    }

    LookAt(lookAtPosition_);

    ControlState<vec3>(moveStateInfo_, CameraState::MOVING, GetPosition(), destinationPosition, time,
                       IsOnDestinationPos());
}
void ThirdPersonCamera::CalculateCameraPosition(float horizontal_distance, float vertical_distance)
{
    float theata   = lookAtRotataion_.y + angleAroundPlayer;
    float x_offset = horizontal_distance * sinf(ToRadians(theata));
    float z_offset = horizontal_distance * cosf(ToRadians(theata));

    destinationPosition.x = lookAtPosition_.x - x_offset;
    destinationPosition.y = lookAtPosition_.y + vertical_distance;
    destinationPosition.z = lookAtPosition_.z - z_offset;
    destinationPosition += offset;
}
float ThirdPersonCamera::CalculateHorizontalDistance()
{
    return distanceFromPlayer * cosf(ToRadians(destinationPitch));
}
float ThirdPersonCamera::CalculateVerticalDistance()
{
    return distanceFromPlayer * sinf(ToRadians(destinationPitch));
}
void ThirdPersonCamera::CalculateYaw()
{
    destinationYaw = 180 - (lookAtRotataion_.y + angleAroundPlayer);
}
void ThirdPersonCamera::CalculateZoom(float zoom_lvl)
{
    this->distanceFromPlayer += zoom_lvl;
}
void ThirdPersonCamera::Lock()
{
    BaseCamera::Lock();
    inputManager.SetReleativeMouseMode(false);
}
void ThirdPersonCamera::Unlock()
{
    BaseCamera::Unlock();
    inputManager.SetReleativeMouseMode(true);
}
vec2 ThirdPersonCamera::CalcualteMouseMove()
{
    auto v = inputManager.CalcualteMouseMove();
    return vec2(v.x, v.y);
}
void ThirdPersonCamera::CalculatePitch(const vec2& d_move)
{
    destinationPitch = destinationPitch + d_move.y;
}

void ThirdPersonCamera::CalculateAngleAroundPlayer(const vec2& d_move)
{
    float angle_change = d_move.x;
    angleAroundPlayer -= angle_change;
}

bool ThirdPersonCamera::IsOnDestinationPos()
{
    auto l = glm::length(GetPosition() - destinationPosition);
    return l < 0.1f;
}

bool ThirdPersonCamera::IsOnDestinationPitch()
{
    auto l = GetYaw() - destinationYaw;

    return fabsf(l) < 0.1f;
}

bool ThirdPersonCamera::IsOnDestinationYaw()
{
    auto l = GetPitch() - destinationPitch;

    return fabsf(l) < 0.1f;
}

template <class T>
T ThirdPersonCamera::CalculateNewValueInTimeInterval(const CameraEvent<T>& t, float time) const
{
    float totalMoveTime = t.endTime - t.startTime;

    if (fabsf(totalMoveTime) < FLT_EPSILON)
    {
        return t.startValue;
    }

    return t.startValue + t.moveValue * (time - t.startTime) / totalMoveTime;
}
}  // namespace GameEngine
