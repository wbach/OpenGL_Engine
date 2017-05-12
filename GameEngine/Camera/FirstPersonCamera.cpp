#include "FirstPersonCamera.h"

CFirstPersonCamera::CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager)
: m_LookPosition(zero)
, m_LookRotation(zero)
, m_Mousevel(0.1f)
, m_Movevel(10.0f)
, m_IsFreeCamera(true)
, m_InputManager(input_manager)
, m_DisplayManager(display_manager)
{
	m_Pitch = 9;
	m_Yaw	= 100;
}

CFirstPersonCamera::CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager, glm::vec3& position_entity, glm::vec3& rotation_entity)
: m_LookPosition(position_entity)
, m_LookRotation(rotation_entity)
, m_Mousevel(0.2f)
, m_Movevel(10.0f)
, m_IsFreeCamera(false)
, m_InputManager(input_manager)
, m_DisplayManager(display_manager)
{
	m_Pitch = 9.0f;
	m_Yaw	= 100.0f;
}

void CFirstPersonCamera::LockCamera()
{
	if(m_Pitch > 90)
		m_Pitch = 90;
	if(m_Pitch < -90)
		m_Pitch = -90;
	if(m_Yaw < 0.0)
		m_Yaw += 360.0;
	if(m_Yaw > 360.0)
		m_Yaw -= 360;
}

void CFirstPersonCamera::Move()
{
	//if (!m_IsFreeCamera)
	//{
	//	m_Position			 = m_LookPosition;
	//	m_Position.y		+= 10;
	//	m_Yaw				 = -m_LookRotation.y +180;

	//	glm::vec2 d_move	 = CalcualteMouseMove(win);
	//	m_LookRotation.y	+= d_move.x;
	//	m_Pitch				-= d_move.y;
	//	LockCamera();
	//	this->UpdateViewMatrix();
	//	return;
	//}

	if (m_InputManager->GetMouseKey(KeyCodes::LMOUSE))
	{
		glm::vec2 dmove = CalcualteMouseMove();
		m_Yaw -= dmove.x;
		m_Pitch -= dmove.y;
		LockCamera();
	}
	//}
	//else
	//{
	//	SDL_ShowCursor(SDL_ENABLE);
	//}

	//const Uint8* state = SDL_GetKeyboardState(NULL);

	float move_velocity = m_Movevel * static_cast<float>(m_DisplayManager->GetDeltaTime());
	if (m_InputManager->GetKey(KeyCodes::UARROW) )
	{
		if(m_Pitch != 90 && m_Pitch != -90)
			MoveCamera(move_velocity, 0.0);
		MoveCameraUp(move_velocity, 0.0);
	}else if (m_InputManager->GetKey(KeyCodes::DARROW))
	{
		if (m_Pitch != 90 && m_Pitch != -90)
			MoveCamera(move_velocity, 180.0);
		MoveCameraUp(move_velocity, 180.0);
	}
	if (m_InputManager->GetKey(KeyCodes::LARROW))
	{
		MoveCamera(-move_velocity, 90.0);
	}
	else if (m_InputManager->GetKey(KeyCodes::RARROW))
	{
		MoveCamera(-move_velocity, 270);
	}

	CCamera::Move();
}
void CFirstPersonCamera::AttachToObject(glm::vec3& position_entity, glm::vec3& rotation_entity) {
	m_LookPosition = position_entity;
	m_LookRotation = rotation_entity;
	m_IsFreeCamera = false;
}
glm::vec2 CFirstPersonCamera::CalcualteMouseMove()
{
	glm::vec2 d_move = m_InputManager->CalcualteMouseMove() * m_Mousevel;
	return d_move;
}

void CFirstPersonCamera::MoveCamera(float dist, float dir)
{
	float rad		 = (m_Yaw +dir)*static_cast<float>(M_PI) / 180.0f;
	m_Position.x	-= sin(-rad)*dist ;
	m_Position.z	-= cos(-rad)*dist;
}

void CFirstPersonCamera::MoveCameraUp(float dist, float dir)
{
	float rad		 = (m_Pitch +dir)*static_cast<float>(M_PI) / 180.0f;
	m_Position.y	+= sin(-rad)*dist;
}
