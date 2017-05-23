#pragma once
#include "Camera.h"
#include <stdio.h>
#include <stdlib.h>
#include "../Input/InputManager.h"
#include "../Display/DisplayManager.hpp"

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

private:
    glm::vec3& lookPosition;
    glm::vec3& lookRotation;


    CInputManager*   inputManager;
    CDisplayManager* displayManager;
};

