#pragma once
#include "Types.h"
#include "Frustrum.h"
#include "Thread.hpp"
#include <atomic>

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

	float GetPitch();
	float GetYaw();
	float GetRoll();
	
	void SetPitch(float pitch);
	void SetRoll(float roll);
	void SetYaw(float yaw);
	
	const vec3 GetDirection() const;
	const vec3& GetPosition();
	const vec3 GetRotation() const;
	const mat4& GetViewMatrix();
	const vec2 GetPositionXZ() const;
	virtual void SetPosition(vec3);

	void UpdateFrustrum();
	bool CheckFrustrumSphereCulling(const vec3& position, const float& radius);

protected:
    float distanceFromPlayer;
    float angleAroundPlayer;

	std::atomic<float> pitch;
	std::atomic<float> yaw;
	std::atomic<float> roll;

    vec3 position;
    mat4 viewMatrix;
	//To frustrum culling
    mat4 projectionMatrix;
	
    CFrustrum frustrum;
	std::mutex viewMatrixMutex;
	std::mutex pmutex;
};
