#pragma once
#include <glm/glm.hpp>
#include "Frustrum.h"

class CCamera
{
public:
	virtual void CalculateInput();
	virtual void Move();
	virtual void CalculateZoom(float zoom_lvl) {}
    virtual void AttachToObject(glm::vec3&, glm::vec3&) {};

	CCamera();
	virtual ~CCamera() {}
	void SetProjectionMatrix(const glm::mat4& projection_matrix);
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
	
	const glm::vec3 GetDirection() const;
	const glm::vec3& GetPosition() const;
	const glm::vec3 GetRotation() const;
	const glm::mat4& GetViewMatrix() const;
	const glm::vec2 GetPositionXZ() const;
	virtual void SetPosition(glm::vec3 position);

	void UpdateFrustrum();
	bool CheckFrustrumSphereCulling(const glm::vec3& position, const float& radius);

protected:
    float distanceFromPlayer;
    float angleAroundPlayer;

    float pitch;
    float yaw;
    float roll;

    glm::vec3 position;
    glm::mat4 viewMatrix;
	//To frustrum culling
    glm::mat4 projectionMatrix;
	
    CFrustrum frustrum;
};
