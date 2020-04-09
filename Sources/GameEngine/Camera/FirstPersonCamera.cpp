#include "FirstPersonCamera.h"

#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "math.hpp"

static vec3 zero(0);

const float defaultCamSpeed         = Utils::KmToMs<float>(135);
const float defaultCamRotationSpeed = 0.2f;

namespace GameEngine
{
FirstPersonCamera::FirstPersonCamera(Input::InputManager* input_manager, GameEngine::DisplayManager* display_manager)
    : FirstPersonCamera(input_manager, display_manager, defaultCamRotationSpeed, defaultCamSpeed, zero, zero, true)
{
}

FirstPersonCamera::FirstPersonCamera(Input::InputManager* input_manager, GameEngine::DisplayManager* display_manager,
                                     float mouse_velocity, float move_velocity)
    : FirstPersonCamera(input_manager, display_manager, mouse_velocity, move_velocity, zero, zero, false)
{
}

FirstPersonCamera::FirstPersonCamera(Input::InputManager* input_manager, GameEngine::DisplayManager* display_manager,
                                     vec3& position_entity, vec3& rotation_entity)
    : FirstPersonCamera(input_manager, display_manager, defaultCamRotationSpeed, defaultCamSpeed, position_entity,
                        rotation_entity, false)
{
}

FirstPersonCamera::FirstPersonCamera(Input::InputManager* input_manager, GameEngine::DisplayManager* display_manager,
                                     float mouse_velocity, float move_velocity, vec3& position_entity,
                                     vec3& rotation_entity, bool freeCamera)
    : BaseCamera(9.f, 100.f)
    , inputManager(input_manager)
    , displayManager(display_manager)
    , lookPosition(position_entity)
    , lookRotation(rotation_entity)
    , isFreeCamera(freeCamera)
    , mousevel(mouse_velocity)
    , movevel(move_velocity)
{
    inputManager->SetReleativeMouseMode(true);
}

void FirstPersonCamera::LockCamera()
{
    LockPitch();
    LockYaw();
}

void FirstPersonCamera::LockPitch()
{
    if (GetRotation().x > 90.f)
        SetPitch(90.f);
    if (GetRotation().x < -90.f)
        SetPitch(-90.f);
}

void FirstPersonCamera::LockYaw()
{
    if (GetRotation().y < 0.f)
        IncreaseYaw(360.f);
    if (GetRotation().y > 360.f)
        IncreaseYaw(-360.f);
}

void FirstPersonCamera::Move()
{
    if (lock_)
        return;

    ApllyMove();
    CalculateMoveVelocity();
    CheckAndProccesDirections();
}

void FirstPersonCamera::Lock()
{
    BaseCamera::Lock();
    inputManager->SetReleativeMouseMode(false);
}

void FirstPersonCamera::Unlock()
{
    BaseCamera::Unlock();
    inputManager->SetReleativeMouseMode(true);
}

vec2 FirstPersonCamera::CalcualteMouseMove()
{
    auto result = inputManager->CalcualteMouseMove();
    return vec2(result.y, result.x) * mousevel;
}

void FirstPersonCamera::ApllyMove()
{
    vec2 dmove = CalcualteMouseMove();
    IncreasePitch(dmove.x);
    IncreaseYaw(dmove.y);
    LockCamera();
}

void FirstPersonCamera::CalculateMoveVelocity()
{
    currentMoveVelocity = movevel * displayManager->GetTime().deltaTime;
}

void FirstPersonCamera::CheckAndProccesDirections()
{
    CheckAndProccesUpDirection();
    CheckAndProccesDownDirection();

    CheckAndProccesLeftDirection();
    CheckAndProccesRightDirection();
}

bool FirstPersonCamera::CheckAndProccesUpDirection()
{
    if (!inputManager->GetKey(KeyCodes::UARROW))
        return false;

    if (GetRotation().x != 90.f and GetRotation().x != -90.f)
        MoveCamera(currentMoveVelocity, 0.f);

    MoveCameraUp(currentMoveVelocity, 0.f);
    return true;
}

bool FirstPersonCamera::CheckAndProccesDownDirection()
{
    if (!inputManager->GetKey(KeyCodes::DARROW))
        return false;

    if (GetRotation().x != 90.f and GetRotation().x != -90.f)
        MoveCamera(currentMoveVelocity, 180.f);

    MoveCameraUp(currentMoveVelocity, 180.f);
    return true;
}

bool FirstPersonCamera::CheckAndProccesLeftDirection()
{
    if (!inputManager->GetKey(KeyCodes::LARROW))
        return false;

    MoveCamera(-currentMoveVelocity, 90.f);
    return true;
}

bool FirstPersonCamera::CheckAndProccesRightDirection()
{
    if (!inputManager->GetKey(KeyCodes::RARROW))
        return false;

    MoveCamera(-currentMoveVelocity, 270.f);
    return true;
}

void FirstPersonCamera::MoveCamera(float dist, float dir)
{
    float rad = ToRadians(GetRotation().y + dir);
    IncreasePositionXZ(vec2(-sinf(-rad) * dist, -cosf(-rad) * dist));
}

void FirstPersonCamera::MoveCameraUp(float dist, float dir)
{
    float rad = ToRadians(GetRotation().x + dir);
    IncreasePositionY(sinf(-rad) * dist);
}
}  // namespace GameEngine
