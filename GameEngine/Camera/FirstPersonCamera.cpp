#include "FirstPersonCamera.h"
#include "../Input/InputManager.h"
#include "../Display/DisplayManager.hpp"

static vec3 zero(0);

CFirstPersonCamera::CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager)
: inputManager(input_manager)
, displayManager(display_manager)
, lookPosition(zero)
, lookRotation(zero)
, isFreeCamera(true)
, mousevel(0.1f)
, movevel(50.0f)
{
	pitch = 9;
	yaw	= 100;
}

CFirstPersonCamera::CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager, vec3& position_entity, vec3& rotation_entity)
: inputManager(input_manager)
, displayManager(display_manager)
, lookPosition(position_entity)
, lookRotation(rotation_entity)
, isFreeCamera(false)
, mousevel(0.2f)
, movevel(50.0f)
{
	pitch = 9.0f;
	yaw	= 100.0f;
}

void CFirstPersonCamera::LockCamera()
{
	if(pitch > 90)
		pitch = 90;
	if(pitch < -90)
		pitch = -90;
	if(yaw < 0.0)
		yaw += 360.0;
	if(yaw > 360.0)
		yaw -= 360;
}

void CFirstPersonCamera::Move()
{
	//if (!m_IsFreeCamera)
	//{
	//	m_Position			 = m_LookPosition;
	//	m_Position.y		+= 10;
	//	m_Yaw				 = -m_LookRotation.y +180;

	//	vec2 d_move	 = CalcualteMouseMove(win);
	//	m_LookRotation.y	+= d_move.x;
	//	m_Pitch				-= d_move.y;
	//	LockCamera();
	//	this->UpdateViewMatrix();
	//	return;
	//}

	if (inputManager->GetMouseKey(KeyCodes::LMOUSE))
	{
		vec2 dmove = CalcualteMouseMove();
		yaw -= dmove.x;
		pitch -= dmove.y;
		LockCamera();
	}
	//}
	//else
	//{
	//	SDL_ShowCursor(SDL_ENABLE);
	//}

	//const Uint8* state = SDL_GetKeyboardState(NULL);

	float move_velocity = movevel * static_cast<float>(displayManager->GetDeltaTime());
	if (inputManager->GetKey(KeyCodes::UARROW) )
	{
		if(pitch != 90 && pitch != -90)
			MoveCamera(move_velocity, 0.0);
		MoveCameraUp(move_velocity, 0.0);
	}else if (inputManager->GetKey(KeyCodes::DARROW))
	{
		if (pitch != 90 && pitch != -90)
			MoveCamera(move_velocity, 180.0);
		MoveCameraUp(move_velocity, 180.0);
	}
	if (inputManager->GetKey(KeyCodes::LARROW))
	{
		MoveCamera(-move_velocity, 90.0);
	}
	else if (inputManager->GetKey(KeyCodes::RARROW))
	{
		MoveCamera(-move_velocity, 270);
	}

	CCamera::Move();
}
void CFirstPersonCamera::AttachToObject(vec3& position_entity, vec3& rotation_entity) {
	lookPosition = position_entity;
	lookRotation = rotation_entity;
	isFreeCamera = false;
}
vec2 CFirstPersonCamera::CalcualteMouseMove()
{
	vec2 d_move = inputManager->CalcualteMouseMove() * mousevel;
	return d_move;
}

void CFirstPersonCamera::MoveCamera(float dist, float dir)
{
	float rad		 = (yaw +dir)*static_cast<float>(M_PI) / 180.0f;
	position.x	-= sin(-rad)*dist ;
	position.z	-= cos(-rad)*dist;
}

void CFirstPersonCamera::MoveCameraUp(float dist, float dir)
{
	float rad		 = (pitch +dir)*static_cast<float>(M_PI) / 180.0f;
	position.y	+= sin(-rad)*dist;
}
