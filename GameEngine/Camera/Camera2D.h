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
	CFirstPersonCamera(CInputManager *input_manager, CDisplayManager *display_manager, vec3& position_entity, vec3& rotation_entity);

	void Move() override;
	void AttachToObject(vec3& position_entity, vec3& rotation_entity) override;

private:
	void MoveCamera(float dist, float dir);
	void MoveCameraUp(float dist, float dir);
	void LockCamera();
	vec2 CalcualteMouseMove();

private:
    vec3& lookPosition;
    vec3& lookRotation;


    CInputManager*   inputManager;
    CDisplayManager* displayManager;
};

