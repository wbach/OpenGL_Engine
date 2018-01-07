#pragma once
#include "Camera.h"

class CInputManager;
class CDisplayManager;

class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager, float& deltaTime);
	CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager, float& deltaTime, float mouse_velocity, float move_velocity);
	CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager, float& deltaTime, vec3& position_entity, vec3& rotation_entity);
	CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager, float& deltaTime, float mouse_velocity, float move_velocity, vec3& position_entity, vec3& rotation_entity, bool freeCamera);

	void Move() override;
	void AttachToObject(vec3& position_entity, vec3& rotation_entity) override;

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
    CInputManager*   inputManager;
    CDisplayManager* displayManager;

    vec3& lookPosition;
    vec3& lookRotation;

    bool isFreeCamera = true;
    float mousevel = 0.f;
    float movevel  = 0.f;

	float currentMoveVelocity;
	float& deltaTime;
};
