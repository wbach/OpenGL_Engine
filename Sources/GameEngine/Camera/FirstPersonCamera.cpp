#include "FirstPersonCamera.h"
#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "math.hpp"

static vec3 zero(0);

const float defaultCamSpeed         = Utils::KmToMs<float>(2500);
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
}

void FirstPersonCamera::LockCamera()
{
    LockPitch();
    LockYaw();
}

void FirstPersonCamera::LockPitch()
{
    if (rotation_.x > 90.f)
        rotation_.x = 90.f;
    if (rotation_.x < -90.f)
        rotation_.x = -90.f;
}

void FirstPersonCamera::LockYaw()
{
    if (rotation_.y < 0.f)
        rotation_.y += 360.f;
    if (rotation_.y > 360.f)
        rotation_.y -= 360.f;
}

void FirstPersonCamera::Move()
{
    if (lock_)
        return;

    ApllyMove();
    CalculateMoveVelocity();
    CheckAndProccesDirections();
}

vec2 FirstPersonCamera::CalcualteMouseMove()
{
    auto result = inputManager->CalcualteMouseMove();
    return vec2(result.x, result.y) * mousevel;
}

void FirstPersonCamera::ApllyMove()
{
    if (not inputManager->GetMouseKey(KeyCodes::LMOUSE) or inputManager->GetKey(KeyCodes::LCTRL))
        return;

    vec2 dmove = CalcualteMouseMove();
    rotation_.y -= dmove.x;
    rotation_.x -= dmove.y;
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

    if (rotation_.x != 90.f and rotation_.x != -90.f)
        MoveCamera(currentMoveVelocity, 0.f);

    MoveCameraUp(currentMoveVelocity, 0.f);
    return true;
}

bool FirstPersonCamera::CheckAndProccesDownDirection()
{
    if (!inputManager->GetKey(KeyCodes::DARROW))
        return false;

    if (rotation_.x != 90.f and rotation_.x != -90.f)
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
    float rad = Utils::ToRadians(rotation_.y + dir);
    position_.x -= sinf(-rad) * dist;
    position_.z -= cosf(-rad) * dist;
}

void FirstPersonCamera::MoveCameraUp(float dist, float dir)
{
    float rad = Utils::ToRadians(rotation_.x + dir);
    position_.y += sinf(-rad) * dist;
}
}  // namespace GameEngine
