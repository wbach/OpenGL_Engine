#pragma once
#include "Camera.h"

class CInputManager;
class CDisplayManager;

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
    CInputManager*   inputManager;
    CDisplayManager* displayManager;

    vec3& lookPosition;
    vec3& lookRotation;

    bool isFreeCamera = true;
    float mousevel = 0.f;
    float movevel  = 0.f;
};
