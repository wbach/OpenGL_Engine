#include "FirstPersonCamera.h"
#include "../Input/InputManager.h"
#include "../Display/DisplayManager.hpp"
#include "math.hpp"

static vec3 zero(0);

const float defaultCamSpeed = Utils::KmToMs<float>(15);
const float defaultCamRotationSpeed = 0.2f;

CFirstPersonCamera::CFirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager *display_manager)
	: CFirstPersonCamera(input_manager, display_manager, defaultCamRotationSpeed, defaultCamSpeed, zero, zero, true)
{
}

CFirstPersonCamera::CFirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager * display_manager, float mouse_velocity, float move_velocity)
	: CFirstPersonCamera(input_manager, display_manager, mouse_velocity, move_velocity, zero, zero, false)
{
}

CFirstPersonCamera::CFirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager *display_manager, vec3& position_entity, vec3& rotation_entity)
	: CFirstPersonCamera(input_manager, display_manager, defaultCamRotationSpeed, defaultCamSpeed, position_entity, rotation_entity, false)
{
}

CFirstPersonCamera::CFirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager * display_manager, float mouse_velocity, float move_velocity, vec3 & position_entity, vec3 & rotation_entity, bool freeCamera)
	: CCamera(9.f, 100.f)
	, inputManager(input_manager)
	, displayManager(display_manager)
	, lookPosition(position_entity)
	, lookRotation(rotation_entity)
	, isFreeCamera(freeCamera)
	, mousevel(mouse_velocity)
	, movevel(move_velocity)
{
}

void CFirstPersonCamera::LockCamera()
{
	LockPitch();
	LockYaw();
}

void CFirstPersonCamera::LockPitch()
{
	float p = pitch.load();

	if (p > 90.f)
		pitch.store(90.f);
	if (p < -90.f)
		pitch.store(-90.f);
}

void CFirstPersonCamera::LockYaw()
{
	float y = yaw.load();

	if (y < 0.f)
		yaw.store(y + 360.f);
	if (y > 360.f)
		yaw.store(y - 360.f);
}

void CFirstPersonCamera::Move()
{
	ApllyMove();
	CalculateMoveVelocity();
    CheckAndProccesDirections();
	CCamera::Move();
}

void CFirstPersonCamera::AttachToObject(vec3& position_entity, vec3& rotation_entity)
{
	lookPosition = position_entity;
	lookRotation = rotation_entity;
	isFreeCamera = false;
}
vec2 CFirstPersonCamera::CalcualteMouseMove()
{
	vec2 d_move = inputManager->CalcualteMouseMove() * mousevel;
	return d_move;
}

void CFirstPersonCamera::ApllyMove()
{
	if (!inputManager->GetMouseKey(KeyCodes::LMOUSE))
		return;

	vec2 dmove = CalcualteMouseMove();
	yaw.store(yaw.load() - dmove.x);// *deltaTime;
	pitch.store(pitch.load() - dmove.y);// *deltaTime;
	LockCamera();
}

void CFirstPersonCamera::CalculateMoveVelocity()
{
	currentMoveVelocity = movevel;// *static_cast<float>(displayManager->GetDeltaTime());
}

void CFirstPersonCamera::CheckAndProccesDirections()
{
	CheckAndProccesUpDirection();
	CheckAndProccesDownDirection();

	CheckAndProccesLeftDirection();
	CheckAndProccesRightDirection();
}

bool CFirstPersonCamera::CheckAndProccesUpDirection()
{
	if (!inputManager->GetKey(KeyCodes::UARROW))
		return false;

	if (pitch.load() != 90.f && pitch.load() != -90.f)
		MoveCamera(currentMoveVelocity, 0.f);

	MoveCameraUp(currentMoveVelocity, 0.f);
	return true;
}

bool CFirstPersonCamera::CheckAndProccesDownDirection()
{
	if (!inputManager->GetKey(KeyCodes::DARROW))
		return false;

	if (pitch.load() != 90.f && pitch.load() != -90.f)
		MoveCamera(currentMoveVelocity, 180.f);

	MoveCameraUp(currentMoveVelocity, 180.f);
	return true;
}

bool CFirstPersonCamera::CheckAndProccesLeftDirection()
{
	if (!inputManager->GetKey(KeyCodes::LARROW))
		return false;

	MoveCamera(-currentMoveVelocity, 90.f);
	return true;
}

bool CFirstPersonCamera::CheckAndProccesRightDirection()
{
	if (!inputManager->GetKey(KeyCodes::RARROW))
		return false;

	MoveCamera(-currentMoveVelocity, 270.f);
	return true;
}

void CFirstPersonCamera::MoveCamera(float dist, float dir)
{
    float rad = (yaw.load() + dir)*static_cast<float>(M_PI) / 180.f;
	position.x -= sin(-rad)*dist;
	position.z -= cos(-rad)*dist;
}

void CFirstPersonCamera::MoveCameraUp(float dist, float dir)
{
    float rad = (pitch.load() + dir)*static_cast<float>(M_PI) / 180.f;
	position.y += sin(-rad)*dist;
}
