#pragma once
#include "Camera.h"

class CTransform;
class CInputManager;

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(CInputManager *input_manager, CTransform& lookAt);

	void CalculateInput() override;
	void Move() override;
	void CalculateZoom(float zoom_lvl) override;
private:
	void LockCamera();
	void SetCaptureMouse(bool capture);	
	void CalculateCameraPosition(float horizontal_distance, float vertical_distance);
	void SetPosition(glm::vec3 position);
	float CalculateHorizontalDistance();
	float CalculateVerticalDistance();

	glm::vec2 CalcualteMouseMove();
	void CalculatePitch(glm::vec2 d_move);
	void CalculateAngleAroundPlayer(glm::vec2 d_move);

private:
    CInputManager *inputManager;
    CTransform&	lookAtTransform;

    bool		isShowCursor;
    glm::vec3	offset;
    float		mousevel;
    bool		captureMouse;
};
