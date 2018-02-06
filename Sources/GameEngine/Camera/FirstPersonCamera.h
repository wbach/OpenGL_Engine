#pragma once
#include "Camera.h"

namespace GameEngine
{
	class CDisplayManager;
	class InputManager;
} // GameEngine

class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager *display_manager);
	CFirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager *display_manager, float mouse_velocity, float move_velocity);
	CFirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager *display_manager, vec3& position_entity, vec3& rotation_entity);
	CFirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager *display_manager, float mouse_velocity, float move_velocity, vec3& position_entity, vec3& rotation_entity, bool freeCamera);

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
	GameEngine::InputManager*   inputManager;
	GameEngine::CDisplayManager* displayManager;

    vec3& lookPosition;
    vec3& lookRotation;

    bool isFreeCamera = true;
    float mousevel = 0.f;
    float movevel  = 0.f;

	float currentMoveVelocity;
};
