#pragma once
#include "Camera.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class DisplayManager;

class FirstPersonCamera : public BaseCamera
{
public:
    FirstPersonCamera(Input::InputManager&, DisplayManager&);
    FirstPersonCamera(Input::InputManager&, DisplayManager&, float mouse_velocity, float move_velocity);
    FirstPersonCamera(Input::InputManager&, DisplayManager&, vec3& position_entity,
                      vec3& rotation_entity);
    FirstPersonCamera(Input::InputManager&, DisplayManager&, float mouse_velocity,
                      float move_velocity, vec3& position_entity, vec3& rotation_entity, bool freeCamera);

    void Move() override;
    void Lock() override;
    void Unlock() override;

private:
    void ApllyMove();
    void CalculateMoveVelocity();
    void CheckAndProccesDirections();
    bool CheckAndProccesUpDirection();
    bool CheckAndProccesDownDirection();
    bool CheckAndProccesLeftDirection();
    bool CheckAndProccesRightDirection();

private:
    void MoveCamera(float dist, float dir);
    void MoveCameraUp(float dist, float dir);
    void LockCamera();
    vec2 CalcualteMouseMove();
    void LockPitch();
    void LockYaw();

private:
    Input::InputManager& inputManager;
    DisplayManager& displayManager;

    vec3& lookPosition;
    vec3& lookRotation;

    bool isFreeCamera = true;
    float mousevel    = 0.f;
    float movevel     = 0.f;

    float currentMoveVelocity;
};
}  // namespace GameEngine
