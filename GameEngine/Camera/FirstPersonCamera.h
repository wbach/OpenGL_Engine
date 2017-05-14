#pragma once
#include "Camera.h"

class CInputManager;
class CDisplayManager;

class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager);
	CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager, glm::vec3& position_entity, glm::vec3& rotation_entity);

	void Move() override;
	void AttachToObject(glm::vec3& position_entity, glm::vec3& rotation_entity) override;
private:
	void MoveCamera(float dist, float dir);
	void MoveCameraUp(float dist, float dir);
	void LockCamera();
	glm::vec2 CalcualteMouseMove();

	float m_Mousevel;
	float m_Movevel;

	glm::vec3& m_LookPosition;
	glm::vec3& m_LookRotation;

	bool m_IsFreeCamera = true;

	CInputManager*   m_InputManager;
	CDisplayManager* m_DisplayManager;
};

