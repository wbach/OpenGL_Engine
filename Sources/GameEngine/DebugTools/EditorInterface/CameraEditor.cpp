#include "CameraEditor.h"

#include <Input/InputManager.h>
#include <Utils/GLM/GLMUtils.h>

#include <Logger/Log.h>
#include "GameEngine/Display/DisplayManager.hpp"

namespace GameEngine
{
namespace
{
const float defaultCamSpeed         = 10.f;
const float defaultCamRotationSpeed = 0.25f;
}  // namespace

CameraEditor::CameraEditor(Input::InputManager& inputManager, DisplayManager& displayManager)
    : Camera(vec3(2.f), vec3(0.f))
    , inputManager_(inputManager)
    , displayManager_(displayManager)
{
    mouseKeyDownSubscribtion_ = inputManager_.SubscribeOnKeyDown(
        KeyCodes::RMOUSE, [&]() { referenceMousePosition_ = inputManager_.GetPixelMousePosition(); });

    mouseKeyUpSubscribtion_ =
        inputManager_.SubscribeOnKeyUp(KeyCodes::RMOUSE, [&]() { referenceMousePosition_ = std::nullopt; });
}

CameraEditor::~CameraEditor()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::RMOUSE, mouseKeyDownSubscribtion_);
    inputManager_.UnsubscribeOnKeyUp(KeyCodes::RMOUSE, mouseKeyUpSubscribtion_);
}

void CameraEditor::Update()
{
    if (referenceMousePosition_)
    {
        auto mouseMove = CalcualteMouseMove() * defaultCamRotationSpeed;
        CalculateYaw(mouseMove.x);
        CalculatePitch(mouseMove.y);
    }

    vec3 moveVector = CalculateInputs();

    if (glm::length(moveVector) > std::numeric_limits<float>::epsilon())
    {
        moveVector = moveVector * glm::normalize(GetRotation().value_);
        moveVector = moveVector * defaultCamSpeed * displayManager_.GetTime().deltaTime;
        IncreasePosition(moveVector);
    }
}

vec3 CameraEditor::CalculateInputs()
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
    if (inputManager_.GetKey(Input::GameAction::CAM_TURN_LEFT))
        IncreaseYaw(-1.f);
    if (inputManager_.GetKey(Input::GameAction::CAM_TURN_RIGHT))
        IncreaseYaw(1.f);
    if (inputManager_.GetKey(Input::GameAction::CAM_PITCH_UP))
        IncreasePitch(-1.f);
    if (inputManager_.GetKey(Input::GameAction::CAM_PITCH_DOWN))
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
    vec2 result;
    auto currentMousePosition = inputManager_.GetPixelMousePosition();
    result.x                  = currentMousePosition.x - referenceMousePosition_->x;
    result.y                  = currentMousePosition.y - referenceMousePosition_->y;
    referenceMousePosition_   = currentMousePosition;
    return result;
}
}  // namespace GameEngine
