#pragma once
#include "Types.h"
#include "Frustrum.h"

class CCamera
{
public:
	virtual void CalculateInput();
	virtual void Move();
	virtual void CalculateZoom(float zoom_lvl) {}
    virtual void AttachToObject(vec3&, vec3&) {};

	CCamera();
	CCamera(float pitch, float yaw);

	virtual ~CCamera() {}
	void SetProjectionMatrix(const mat4& projection_matrix);
	void UpdateViewMatrix();
	void InvertPitch();	

	const float& GetDistance() const;
	const float& GetAngleAround() const;

	void SetAngleAround(float a);
	void SetDistance(float dist);
	void SetPitch(float pitch);

	const float& GetPitch() const;
	const float& GetYaw() const;
	const float& GetRoll() const;
	
	void SetRoll(float roll);
	void SetYaw(float yaw);
	
	const vec3 GetDirection() const;
	const vec3& GetPosition() const;
	const vec3 GetRotation() const;
	const mat4& GetViewMatrix() const;
	const vec2 GetPositionXZ() const;
	virtual void SetPosition(vec3 position);

	void UpdateFrustrum();
	bool CheckFrustrumSphereCulling(const vec3& position, const float& radius);

protected:
    float distanceFromPlayer;
    float angleAroundPlayer;

    float pitch;
    float yaw;
    float roll;

    vec3 position;
    mat4 viewMatrix;
	//To frustrum culling
    mat4 projectionMatrix;
	
    CFrustrum frustrum;
};
