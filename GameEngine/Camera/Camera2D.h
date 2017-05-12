#pragma once
#include "Camera.h"
#include <stdio.h>
#include <stdlib.h>
#include "../Input/InputManager.h"
#include "../Display/DisplayManager.hpp"

static glm::vec3 zero(0);

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

	glm::vec3& m_LookPosition;
	glm::vec3& m_LookRotation;


	CInputManager*   m_InputManager;
	CDisplayManager* m_DisplayManager;
};

