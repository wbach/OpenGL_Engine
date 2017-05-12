#pragma once
#include "glm/glm.hpp"
#include "Frustrum.h"

class CCamera
{
public:
	virtual void CalculateInput();
	virtual void Move();
	virtual void CalculateZoom(float zoom_lvl) {}
	virtual void AttachToObject(glm::vec3& position_entity, glm::vec3& rotation_entity) {};

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
	float m_DistanceFromPlayer;
	float m_AngleAroundPlayer;

	float m_Pitch;
	float m_Yaw;
	float m_Roll;

	glm::vec3 m_Position;
	glm::mat4 m_ViewMatrix;
	//To frustrum culling
	glm::mat4 m_ProjectionMatrix;
	
	CFrustrum m_Frustrum;
};
