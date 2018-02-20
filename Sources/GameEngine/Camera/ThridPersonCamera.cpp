#include "ThridPersonCamera.h"
#include "Common/Transform.h"
#include "GameEngine/Input/InputManager.h"
#include "Utils.h"
#include "Logger/Log.h"
#include "GLM/GLMUtils.h"
#include <algorithm>
#include <cmath>

#define DurationToFloatMs(x) std::chrono::duration<float, std::milli>(x).count()

CThirdPersonCamera::CThirdPersonCamera(GameEngine::InputManager* input_manager, common::Transform* look_at)
	: inputManager(input_manager)
	, isShowCursor(false)
	, offset(0.f, 1.8f, 0.f)
	, mousevel(0.5f)
	, moveTime(.125f)
	, captureMouse(true)
	, clock(std::chrono::milliseconds(5))
	, destinationPosition(0)
	, destinationYaw(0)
	, destinationPitch(0)
	, lookAt_(look_at)
{	
	distanceFromPlayer = 2.0f;
	referenceTime = std::chrono::high_resolution_clock::now();
}

CThirdPersonCamera::~CThirdPersonCamera()
{
}

void CThirdPersonCamera::SetCaptureMouse(bool capture)
{
	captureMouse = capture;
}

void CThirdPersonCamera::LockPitch()
{
	if (destinationPitch > 90.0f)
		destinationPitch = (90.0f);
	if (destinationPitch < -90.0f)
		destinationPitch = (-90.0f);
}

void CThirdPersonCamera::LockYaw()
{
	if (destinationYaw < 0.0f)
		destinationYaw += 360.0f;
	if (destinationYaw > 360.0f)
		destinationYaw -= 360.0f;
}

void CThirdPersonCamera::StaticCameraMove()
{
	pitch.store(destinationPitch);
	yaw.store(destinationYaw);
	SetPosition(destinationPosition);
}

void CThirdPersonCamera::LockCamera()
{
	LockPitch();
	LockYaw();
}

void CThirdPersonCamera::CalculateInput()
{
	if (inputManager->GetKey(KeyCodes::LCTRL))
	{
		inputManager->ShowCursor(true);
		return;
	}

	inputManager->ShowCursor(false);

	if (!clock.OnTick())
		return;

	vec2 d_move = CalcualteMouseMove() * mousevel;
	CalculatePitch(d_move);
	CalculateAngleAroundPlayer(d_move);
}

float CThirdPersonCamera::GetTime() const
{
	auto currnet = std::chrono::high_resolution_clock::now() - referenceTime;
	return DurationToFloatMs(currnet) / 1000.0f;
}

bool CThirdPersonCamera::FindState(CameraState state)
{
	return std::find(states_.begin(), states_.end(), state) != states_.end();
}

template<class T>
T CThirdPersonCamera::ProcessState(CameraEvent<T>& stateInfo, const T& destination, float time, bool& remove)
{
	auto newValue = CalculateNewValueInTimeInterval<T>(stateInfo, time);	

	float dt = (stateInfo.endTime - time) / moveTime;
	stateInfo.startValue = newValue;
	stateInfo.moveValue = destination - stateInfo.startValue;
	stateInfo.moveValue *= dt;
	stateInfo.startTime = time;

	if (time > stateInfo.endTime)
		remove = true;
	else
		remove = false;

	return newValue;
}

template<class T>
void CThirdPersonCamera::ControlState(CameraEvent<T>& stateInfo, CameraState state, const T& startValue, const T& destination, float time, bool use)
{
	if (!use)
	{
		if (!FindState(state))
		{
			states_.push_back(state);
			stateInfo.startValue = startValue;
			stateInfo.moveValue = destination - stateInfo.startValue;
			stateInfo.startTime = GetTime();
			stateInfo.endTime = stateInfo.startTime + moveTime;
		}
	}
	else
	{
		if (FindState(state))
		{
			if (time > stateInfo.endTime)
			{
				states_.remove(state);
			}
		}
	}
}

void CThirdPersonCamera::Move()
{
	lookAtPosition_ = lookAt_->GetSnapShoot().position;
	lookAtRotataion_ = lookAt_->GetSnapShoot().rotation;

	float horizontal_distance = CalculateHorizontalDistance();
	float vertical_distance = CalculateVerticalDistance();
	CalculateCameraPosition(horizontal_distance, vertical_distance);
	CalculateYaw();

	StaticCameraMove();
	//SmoothCameraMove();

	LockCamera();
}

void CThirdPersonCamera::SmoothCameraMove()
{
	float time = GetTime();

	for (auto state = states_.begin(); state != states_.end(); )
	{
		bool remove = false;

		switch (*state)
		{
		case CameraState::MOVING:
		{
			auto newPos = ProcessState<vec3>(moveStateInfo_, destinationPosition, time, remove);
			SetPosition(newPos);
			break;
		}
		}

		if (remove)
		{
			state = states_.erase(state);
		}
		else
		{
			++state;
		}
	}

	auto xyz = GetPosition() - lookAtPosition_;
	float yalpha = Utils::ToDegrees(atan2(xyz.z, xyz.x) - static_cast<float>(M_PI) / 2.f);
	float palpha = Utils::ToDegrees(atan2(xyz.y, sqrt(xyz.x * xyz.x + xyz.z*xyz.z)));

	pitch.store(palpha);
	yaw.store(yalpha);
	ControlState<vec3>(moveStateInfo_, CameraState::MOVING, GetPosition(), destinationPosition, time, IsOnDestinationPos());
}

void CThirdPersonCamera::SetPosition(vec3 position_)
{
	CCamera::SetPosition(position_);
}
void CThirdPersonCamera::CalculateCameraPosition(float horizontal_distance, float vertical_distance)
{
	float theata = lookAtRotataion_.y + angleAroundPlayer;
	float x_offset = (float) (horizontal_distance * sin(Utils::ToRadians(theata)));
	float z_offset = (float) (horizontal_distance * cos(Utils::ToRadians(theata)));

	destinationPosition.x = lookAtPosition_.x - x_offset;
	destinationPosition.y = lookAtPosition_.y + vertical_distance;
	destinationPosition.z = lookAtPosition_.z - z_offset;
	destinationPosition += offset;
}

float CThirdPersonCamera::CalculateHorizontalDistance()
{
	return (float) (distanceFromPlayer * cos(Utils::ToRadians(destinationPitch)));
}

float CThirdPersonCamera::CalculateVerticalDistance()
{
	return (float) (distanceFromPlayer * sin(Utils::ToRadians(destinationPitch)));
}

void CThirdPersonCamera::CalculateYaw()
{
	destinationYaw = 180 - (lookAtRotataion_.y + angleAroundPlayer);
}

void CThirdPersonCamera::CalculateZoom(float zoom_lvl)
{
	this->distanceFromPlayer += zoom_lvl;
}

void CThirdPersonCamera::SetLookAtTransform(common::Transform * lookAt)
{
	lookAt_ = lookAt;
}

vec2 CThirdPersonCamera::CalcualteMouseMove()
{
	return inputManager->CalcualteMouseMove();
}

void CThirdPersonCamera::CalculatePitch(vec2 d_move)
{
	float p = pitch.load();

	destinationPitch = destinationPitch - d_move.y;
}

void CThirdPersonCamera::CalculateAngleAroundPlayer(vec2 d_move)
{
	float angle_change = d_move.x;
	angleAroundPlayer -= angle_change;
}

bool CThirdPersonCamera::IsOnDestinationPos()
{
	auto l = glm::length(GetPosition() - destinationPosition);

	return l < 0.1f;
}

bool CThirdPersonCamera::IsOnDestinationPitch()
{
	auto l = yaw.load() - destinationYaw;

	return fabs(l) < 0.1f;
}

bool CThirdPersonCamera::IsOnDestinationYaw()
{
	auto l = pitch.load() - destinationPitch;

	return fabs(l) < 0.1f;
}

template<class T>
T CThirdPersonCamera::CalculateNewValueInTimeInterval(const CameraEvent<T>& t, float time) const
{
	float totalMoveTime = t.endTime - t.startTime;

	if (fabs(totalMoveTime) < FLT_EPSILON)
	{
		return t.startValue;
	}

	return t.startValue + t.moveValue * (time - t.startTime) / totalMoveTime;
}
