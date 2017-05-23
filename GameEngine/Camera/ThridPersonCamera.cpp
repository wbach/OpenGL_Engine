#include "ThridPersonCamera.h"
#include "../Utils/Utils.h"
#include "../Input/InputManager.h"
#include "../Objects/Transform.h"

CThirdPersonCamera::CThirdPersonCamera(CInputManager* input_manager, CTransform& look_at)
: inputManager(input_manager)
, lookAtTransform(look_at)
, isShowCursor(false)
, offset(0.0f)
, mousevel(0.2f)
, captureMouse(true)
{
	distanceFromPlayer = 4.0f;
}

void CThirdPersonCamera::SetCaptureMouse(bool capture)
{
	captureMouse = capture;
}
void CThirdPersonCamera::LockCamera()
{
	if(pitch > 90.0f)
		pitch = 90.0f;
	if(pitch < -90.0f)
		pitch = -90.0f;
	if(yaw < 0.0f)
		yaw += 360.0f;
	if(yaw > 360.0f)
		yaw -= 360.0f;
}

void CThirdPersonCamera::CalculateInput()
{
	glm::vec2 d_move = CalcualteMouseMove() * mousevel;
	CalculatePitch(d_move);
	CalculateAngleAroundPlayer(d_move);
	LockCamera();
}

void CThirdPersonCamera::Move()
{
	float horizontal_distance = CalculateHorizontalDistance() ;
	float vertical_distance	 = CalculateVerticalDistance() ;
	CalculateCameraPosition(horizontal_distance, vertical_distance);
	this->yaw = 180 - (lookAtTransform.GetRotation().y + angleAroundPlayer) ;

	CCamera::Move();
}
void CThirdPersonCamera::SetPosition(glm::vec3 position)
{
	position = position;
}
void CThirdPersonCamera::CalculateCameraPosition(float horizontal_distance, float vertical_distance)
{
	float theata  = lookAtTransform.GetRotation().y  + angleAroundPlayer;
	float x_offset = (float) (horizontal_distance * sin(Utils::ToRadians(theata))) ;
	float z_offset = (float) (horizontal_distance * cos(Utils::ToRadians(theata))) ;
	position.x  = lookAtTransform.GetPosition().x - x_offset;
	position.y  = lookAtTransform.GetPosition().y + vertical_distance + 1.8f;
	position.z  = lookAtTransform.GetPosition().z - z_offset;
	position	 += offset;
}

float CThirdPersonCamera::CalculateHorizontalDistance()
{
	return (float) (distanceFromPlayer * cos(Utils::ToRadians(pitch))) ;
}

float CThirdPersonCamera::CalculateVerticalDistance()
{
	return (float) (distanceFromPlayer * sin(Utils::ToRadians(pitch))) ;
}

void CThirdPersonCamera::CalculateZoom(float zoom_lvl)
{
	this->distanceFromPlayer += zoom_lvl;
}

glm::vec2 CThirdPersonCamera::CalcualteMouseMove()
{
	return inputManager->CalcualteMouseMove();
}

void CThirdPersonCamera::CalculatePitch(glm::vec2 d_move)
{
		pitch -= d_move.y;
}

void CThirdPersonCamera::CalculateAngleAroundPlayer(glm::vec2 d_move)
{
	float angle_change	 = d_move.x;
	angleAroundPlayer -= angle_change;
}
