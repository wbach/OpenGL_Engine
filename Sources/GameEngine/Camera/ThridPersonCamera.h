#pragma once
#include "Camera.h"
#include "Clock.hpp"

namespace common
{
	class Transform;
} // common

namespace GameEngine
{
	class InputManager;
} // GameEngine

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(GameEngine::InputManager* input_manager, common::Transform& lookAt);

	void CalculateInput() override;
	void Move() override;
	void CalculateZoom(float zoom_lvl) override;

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

private:
	GameEngine::InputManager* inputManager;
	common::Transform&	lookAtTransform;

    bool	isShowCursor;
    vec3	offset;
    float	mousevel;
    bool	captureMouse;

	Utils::CClock clock;
};
