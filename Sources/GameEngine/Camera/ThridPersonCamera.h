#pragma once
#include "Camera.h"

class CTransform;
class CInputManager;

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(CInputManager *input_manager, CTransform& lookAt, float* deltaTime = nullptr);

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
    CInputManager *inputManager;
    CTransform&	lookAtTransform;

    bool	isShowCursor;
    vec3	offset;
    float	mousevel;
    bool	captureMouse;
	float*  deltaTime;
};
