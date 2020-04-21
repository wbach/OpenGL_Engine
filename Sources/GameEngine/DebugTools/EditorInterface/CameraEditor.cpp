#include "CameraEditor.h"
#include <Input/InputManager.h>
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/EngineContext.h"

namespace GameEngine
{
namespace
{
const float MOVE_SPEED = 10.f;
}
CameraEditor::CameraEditor(Input::InputManager& inputManager, DisplayManager& displayManager)
    : BaseCamera(vec3(2.f), vec3(0.f))
    , inputManager_(inputManager)
    , displayManager_(displayManager)
    , cameraPosiionDebug_(EngineContext.AddNewMeasurment("cameraPosition"))
    , cameraRotationDebug_(EngineContext.AddNewMeasurment("cameraRotation"))
{
}

void CameraEditor::Move()
{
    if (lock_)
        return;

    LockPitch();
    LockYaw();

    vec3 moveVector(0.f);
    if (inputManager_.GetKey(Input::GameAction::MOVE_LEFT))
        moveVector.x = 1;
    if (inputManager_.GetKey(Input::GameAction::MOVE_RIGHT))
        moveVector.x = -1;
    if (inputManager_.GetKey(Input::GameAction::MOVE_FORWARD))
        moveVector.z = 1;
    if (inputManager_.GetKey(Input::GameAction::MOVE_BACKWARD))
        moveVector.z = -1;

    moveVector = GetRotation() * moveVector;
    moveVector = glm::normalize(moveVector);
    moveVector *= MOVE_SPEED;
    IncreasePosition(moveVector);
    // currentMoveVelocity = MOVE_SPEED * displayManager_.GetTime().deltaTime;

    cameraPosiionDebug_ = std::to_string(GetPosition());
    cameraRotationDebug_ = std::to_string(GetRotation());
}

void CameraEditor::Lock()
{
}

void CameraEditor::Unlock()
{
}

void CameraEditor::LockPitch()
{
    if (GetRotation().x > 90.f)
        SetPitch(90.f);
    if (GetRotation().x < -90.f)
        SetPitch(-90.f);
}

void CameraEditor::LockYaw()
{
    if (GetRotation().y < 0.f)
        IncreaseYaw(360.f);
    if (GetRotation().y > 360.f)
        IncreaseYaw(-360.f);
}

}  // namespace GameEngine
