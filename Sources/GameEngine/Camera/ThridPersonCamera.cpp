#include "ThridPersonCamera.h"
#include "../Input/InputManager.h"
#include "../../Common/Transform.h"
#include "Utils.h"

CThirdPersonCamera::CThirdPersonCamera(GameEngine::InputManager* input_manager, common::Transform& look_at)
	: inputManager(input_manager)
	, lookAtTransform(look_at)
	, isShowCursor(false)
	, offset(0.f, 1.0f, 0.f)
	, mousevel(0.5f)
	, captureMouse(true)
	, clock(std::chrono::milliseconds(5))
{
	distanceFromPlayer = 12.0f;
}

void CThirdPersonCamera::SetCaptureMouse(bool capture)
{
	captureMouse = capture;
}

void CThirdPersonCamera::LockPitch()
{
	float p = pitch.load();

	if (p > 90.0f)
		pitch.store(90.0f);
	if (p < -90.0f)
		pitch.store(-90.0f);
}

void CThirdPersonCamera::LockYaw()
{
	float y = yaw.load();

	if (y < 0.0f)
		yaw.store(y + 360.0f);
	if (y > 360.0f)
		yaw.store(y - 360.0f);
}

void CThirdPersonCamera::LockCamera()
{
	LockPitch();
	LockYaw();
}

void CThirdPersonCamera::CalculateInput()
{
	if (!clock.OnTick())
		return;

	vec2 d_move = CalcualteMouseMove() * mousevel;
	CalculatePitch(d_move);
	CalculateAngleAroundPlayer(d_move);
	LockCamera();
}

void CThirdPersonCamera::Move()
{
	float horizontal_distance = CalculateHorizontalDistance();
	float vertical_distance = CalculateVerticalDistance();
	CalculateCameraPosition(horizontal_distance, vertical_distance);
	CalculateYaw();

	CCamera::Move();
}
void CThirdPersonCamera::SetPosition(vec3 position_)
{
	CCamera::SetPosition(position_);
}
void CThirdPersonCamera::CalculateCameraPosition(float horizontal_distance, float vertical_distance)
{
	float theata = lookAtTransform.GetRotation().y + angleAroundPlayer;
	float x_offset = (float) (horizontal_distance * sin(Utils::ToRadians(theata)));
	float z_offset = (float) (horizontal_distance * cos(Utils::ToRadians(theata)));

	vec3 pos;
	pos.x = lookAtTransform.GetPosition().x - x_offset;
	pos.y = lookAtTransform.GetPosition().y + vertical_distance + 1.8f;
	pos.z = lookAtTransform.GetPosition().z - z_offset;
	pos += offset;

	SetPosition(pos);
}

float CThirdPersonCamera::CalculateHorizontalDistance()
{
	return (float) (distanceFromPlayer * cos(Utils::ToRadians(pitch.load())));
}

float CThirdPersonCamera::CalculateVerticalDistance()
{
	return (float) (distanceFromPlayer * sin(Utils::ToRadians(pitch.load())));
}

void CThirdPersonCamera::CalculateYaw()
{
	yaw.store( 180 - (lookAtTransform.GetRotation().y + angleAroundPlayer) );
}

void CThirdPersonCamera::CalculateZoom(float zoom_lvl)
{
	this->distanceFromPlayer += zoom_lvl;
}

vec2 CThirdPersonCamera::CalcualteMouseMove()
{
	return inputManager->CalcualteMouseMove();
}

void CThirdPersonCamera::CalculatePitch(vec2 d_move)
{
	float p = pitch.load();
	pitch.store(p - d_move.y);
}

void CThirdPersonCamera::CalculateAngleAroundPlayer(vec2 d_move)
{
	float angle_change = d_move.x;
	angleAroundPlayer -= angle_change;
}
