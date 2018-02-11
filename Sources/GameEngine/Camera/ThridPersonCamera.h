#pragma once
#include "Camera.h"
#include "Clock.hpp"
#include <list>

namespace common
{
	class Transform;
} // common

namespace GameEngine
{
	class InputManager;
} // GameEngine

enum CameraState
{
	MOVING,
	ROTATE_PITCH,
	ROTATE_YAW,
	SHAKING
};

template<class T>
struct CameraEvent
{
	T startValue;
	T moveValue;
	float startTime;
	float endTime;
};

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(GameEngine::InputManager* input_manager, common::Transform& lookAt);

	void CalculateInput() override;
	void Move() override;
	void CalculateZoom(float zoom_lvl) override;
	void OnLookAtChange(const vec3& pos, const vec3& rot, const vec3& scale, const mat4&);

private:
	void LockCamera();
	void SetCaptureMouse(bool capture);
	void LockPitch();
	void LockYaw();
	void CalculateCameraPosition(float horizontal_distance, float vertical_distance);
	void SetPosition(vec3 position);
	float CalculateHorizontalDistance();
	float CalculateVerticalDistance();
	void CalculateYaw();
	vec2 CalcualteMouseMove();
	void CalculatePitch(vec2 d_move);
	void CalculateAngleAroundPlayer(vec2 d_move);
	bool IsOnDestinationPos();
	bool IsOnDestinationPitch();
	bool IsOnDestinationYaw();

	float GetTime() const;
	bool FindState(CameraState state);

	template<class T>
	T CalculateNewValueInTimeInterval(const CameraEvent<T>& t, float time) const
	{
		float totalMoveTime = t.endTime - t.startTime;

		if (fabs(totalMoveTime) < FLT_EPSILON)
		{
			return t.startValue;
		}

		return t.startValue + t.moveValue * (time - t.startTime) / totalMoveTime;
	}

	template<class T>
	T ProcessState(CameraEvent<T>& stateInfo, const T& destination, float time, bool& remove);

	template<class T>
	void ControlState(CameraEvent<T>& stateInfo, CameraState state, const T& startValue, const T& destination, float time, bool use);

private:
	GameEngine::InputManager* inputManager;
	
	vec3 lookAtPosition;
	vec3 lookAtRotataion;

    bool	isShowCursor;
    vec3	offset;
    float	mousevel;
    bool	captureMouse;

	float destinationYaw;
	float destinationPitch;

	float moveTime;
	vec3 destinationPosition;
	
	CameraEvent<vec3> moveStateInfo_;
	CameraEvent<float> rotatePitchStateInfo_;
	CameraEvent<float> rotateYawStateInfo_;

	Timepoint referenceTime;
	std::list<CameraState> states_;

	Utils::CClock clock;
};
