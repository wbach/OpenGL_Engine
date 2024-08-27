#include "FirstPersonCamera.h"

#include <Input/InputManager.h>

#include <Utils/math.hpp>

#include "GameEngine/Display/DisplayManager.hpp"

namespace GameEngine
{
namespace
{
const float defaultCamRotationSpeed = 0.2f;
const float defaultCamSpeed         = Utils::KmToMs<float>(60);
}  // namespace

FirstPersonCamera::FirstPersonCamera(Input::InputManager& inputManager, GameEngine::DisplayManager& displayManager)
    : Camera(vec3(2.f), vec3(0.f))
    , inputManager_(inputManager)
    , displayManager_(displayManager)
    , moveLock_(false)
{
    inputManager_.SetReleativeMouseMode(true);
}

void FirstPersonCamera::Update()
{
    auto mouseMove = CalcualteMouseMove() * defaultCamRotationSpeed;
    CalculateYaw(mouseMove.x);
    CalculatePitch(mouseMove.y);

    if (not moveLock_)
    {
        vec3 moveVector = CalculateInputs();

        if (glm::length(moveVector) > std::numeric_limits<float>::epsilon())
        {
            moveVector = moveVector * glm::normalize(GetRotation().value_);
            moveVector = moveVector * defaultCamSpeed * displayManager_.GetTime().deltaTimeNoModified;  // for freecam
            IncreasePosition(moveVector);
        }
    }
}

vec3 FirstPersonCamera::CalculateInputs()
{
    vec3 moveVector(0.f);
    if (inputManager_.GetKey(Input::GameAction::CAM_FORWARD))
        moveVector.z = -1;
    if (inputManager_.GetKey(Input::GameAction::CAM_BACKWARD))
        moveVector.z = 1;
    if (inputManager_.GetKey(Input::GameAction::CAM_LEFT))
        moveVector.x = -1;
    if (inputManager_.GetKey(Input::GameAction::CAM_RIGHT))
        moveVector.x = 1;
    if (inputManager_.GetKey(Input::GameAction::CAM_LEFT))
        IncreaseYaw(-1.f);
    if (inputManager_.GetKey(Input::GameAction::CAM_RIGHT))
        IncreaseYaw(1.f);
    if (inputManager_.GetKey(Input::GameAction::CAM_PITCH_UP))
        IncreasePitch(-1.f);
    if (inputManager_.GetKey(Input::GameAction::CAM_PITCH_DOWN))
        IncreasePitch(1.f);
    return moveVector;
}

void FirstPersonCamera::CalculatePitch(float v)
{
    IncreasePitch(v);
}

void FirstPersonCamera::CalculateYaw(float v)
{
    IncreaseYaw(v);
}

vec2 FirstPersonCamera::CalcualteMouseMove()
{
    auto v = inputManager_.CalcualteMouseMove();
    return vec2(v.x, v.y);
}

void FirstPersonCamera::Lock()
{
    Camera::Lock();
    inputManager_.SetReleativeMouseMode(false);
    inputManager_.ShowCursor(true);
}

void FirstPersonCamera::Unlock()
{
    Camera::Unlock();
    inputManager_.SetReleativeMouseMode(true);
    inputManager_.ShowCursor(false);
}
}  // namespace GameEngine
