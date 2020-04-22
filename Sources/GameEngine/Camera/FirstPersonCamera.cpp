#include "FirstPersonCamera.h"

#include <Input/InputManager.h>

#include "GameEngine/Display/DisplayManager.hpp"

namespace GameEngine
{
namespace
{
const float defaultCamRotationSpeed = 0.2f;
const float defaultCamSpeed         = Utils::KmToMs<float>(135);
}  // namespace

FirstPersonCamera::FirstPersonCamera(Input::InputManager& inputManager, GameEngine::DisplayManager& displayManager)
    : BaseCamera(vec3(2.f), vec3(0.f))
    , inputManager_(inputManager)
    , displayManager_(displayManager)
{
    inputManager_.SetReleativeMouseMode(true);
}

void FirstPersonCamera::Move()
{
    if (lock_)
        return;

    auto mouseMove = CalcualteMouseMove() * defaultCamRotationSpeed;
    CalculateYaw(mouseMove.x);
    CalculatePitch(mouseMove.y);

    vec3 moveVector = CalculateInputs();

    if (glm::length(moveVector) > std::numeric_limits<float>::epsilon())
    {
        moveVector = moveVector * glm::normalize(GetRotation().value_);
        moveVector = moveVector * defaultCamSpeed * displayManager_.GetTime().deltaTime;
        IncreasePosition(moveVector);
    }
}

vec3 FirstPersonCamera::CalculateInputs()
{
    vec3 moveVector(0.f);
    if (inputManager_.GetKey(Input::GameAction::MOVE_FORWARD))
        moveVector.z = -1;
    if (inputManager_.GetKey(Input::GameAction::MOVE_BACKWARD))
        moveVector.z = 1;
    if (inputManager_.GetKey(Input::GameAction::MOVE_LEFT))
        moveVector.x = -1;
    if (inputManager_.GetKey(Input::GameAction::MOVE_RIGHT))
        moveVector.x = 1;
    if (inputManager_.GetKey(Input::GameAction::TURN_LEFT))
        IncreaseYaw(-1.f);
    if (inputManager_.GetKey(Input::GameAction::TURN_RIGHT))
        IncreaseYaw(1.f);
    if (inputManager_.GetKey(Input::GameAction::INCREASE_PITCH))
        IncreasePitch(-1.f);
    if (inputManager_.GetKey(Input::GameAction::INCREASE_YAW))
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
    BaseCamera::Lock();
    inputManager_.SetReleativeMouseMode(false);
    inputManager_.ShowCursor(true);
}

void FirstPersonCamera::Unlock()
{
    BaseCamera::Unlock();
    inputManager_.SetReleativeMouseMode(true);
    inputManager_.ShowCursor(false);
}
}  // namespace GameEngine
