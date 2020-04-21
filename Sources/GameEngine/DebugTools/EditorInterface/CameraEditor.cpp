#include "CameraEditor.h"
#include <Input/InputManager.h>
#include <Utils/GLM/GLMUtils.h>
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/EngineContext.h"

namespace GameEngine
{
namespace
{
const float MOVE_SPEED = 1.f;
}
CameraEditor::CameraEditor(Input::InputManager& inputManager, DisplayManager& displayManager)
    : BaseCamera(0.f, 0.f)
    , inputManager_(inputManager)
    , displayManager_(displayManager)
    , cameraPosiionDebug_(EngineContext.AddNewMeasurment("p"))
    , cameraRotationDebug_(EngineContext.AddNewMeasurment("r"))
    , moveVector_(EngineContext.AddNewMeasurment("v"))
{
    SetPosition(vec3(2.f));
    UpdateMatrix();
}

void CameraEditor::Move()
{
    LockPitch();
    LockYaw();

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

    if (inputManager_.GetKey(KeyCodes::Z))
        IncreasePitch(-1.f);
    if (inputManager_.GetKey(KeyCodes::X))
        IncreasePitch(1.f);


    if (glm::length(moveVector) > std::numeric_limits<float>::epsilon())
    {
        DegreesVec3 v(GetRotation());
        glm::quat qPitch = glm::angleAxis(-v.Radians().x, glm::vec3(1, 0, 0));
        glm::quat qYaw   = glm::angleAxis(-v.Radians().y, glm::vec3(0, 1, 0));
        glm::quat qRoll  = glm::angleAxis(-v.Radians().z, glm::vec3(0, 0, 1));

        // For a FPS camera we can omit roll
        glm::quat orientation = qPitch * qYaw * qRoll;
        orientation           = glm::normalize(orientation);

        moveVector = orientation * moveVector;
        moveVector = moveVector * MOVE_SPEED * displayManager_.GetTime().deltaTime;
        IncreasePosition(moveVector);
    }

    cameraPosiionDebug_  = std::to_string(GetPosition());
    cameraRotationDebug_ = std::to_string(GetRotation());
    moveVector_          = std::to_string(moveVector);
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
