#include "CameraEditor.h"

#include <Input/InputManager.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Display/DisplayManager.hpp"

namespace GameEngine
{
namespace
{
const float defaultCamSpeed         = 10.f;
const float defaultCamRotationSpeed = 0.25f;
}  // namespace

CameraEditor::CameraEditor(Input::InputManager& inputManager, DisplayManager& displayManager)
    : BaseCamera(vec3(2.f), vec3(0.f))
    , inputManager_(inputManager)
    , displayManager_(displayManager)
{
}

void CameraEditor::Move()
{
    if (lock_)
        return;

    auto mouseMove = CalcualteMouseMove() * defaultCamRotationSpeed;
    CalculateYaw(mouseMove.x);
    CalculatePitch(mouseMove.y);

    vec3 moveVector = CalculateInputs();

    if (glm::length(moveVector) > std::numeric_limits<float>::epsilon())
    {
        moveVector = moveVector * glm::normalize(rotation_.value_);
        moveVector = moveVector * defaultCamSpeed * displayManager_.GetTime().deltaTime;
        IncreasePosition(moveVector);
    }
}

vec3 CameraEditor::CalculateInputs()
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

void CameraEditor::CalculatePitch(float v)
{
    IncreasePitch(v);
}

void CameraEditor::CalculateYaw(float v)
{
    IncreaseYaw(v);
}

vec2 CameraEditor::CalcualteMouseMove()
{
    auto v = inputManager_.CalcualteMouseMove();
    return vec2(v.x, v.y);
}
}  // namespace GameEngine
