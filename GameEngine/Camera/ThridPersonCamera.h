#pragma once
#include "../Input/InputManager.h"
#include "../Objects/Transform.h"
#include "Camera.h"

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

	CTransform&	m_LookAtTransform;

	bool		m_IsShowCursor;
	glm::vec3	m_Offset;
	float		m_Mousevel;
	bool		m_CaptureMouse;
	int			m_WindowWidth, m_WindowHeight;
	CInputManager *m_InputManager;
};
