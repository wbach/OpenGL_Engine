#include "Camera.h"
#include "../Utils/Utils.h"
#include "../Utils/GLM/GLMUtils.h"

CCamera::CCamera() 
: m_DistanceFromPlayer(99)
, m_AngleAroundPlayer(0)
, m_Position(0)
, m_Pitch(20)
, m_Yaw(0)
, m_Roll(0)
{	
	UpdateViewMatrix();
	UpdateFrustrum();
}

void CCamera::SetProjectionMatrix(const glm::mat4 & projection_matrix)
{
	m_ProjectionMatrix = projection_matrix;
}

void CCamera::SetPosition(glm::vec3 position)
{
	m_Position = position;
}
void CCamera::UpdateFrustrum()
{
	m_Frustrum.CalculatePlanes(m_ProjectionMatrix * m_ViewMatrix);
}
bool CCamera::CheckFrustrumSphereCulling(const glm::vec3 & position, const float& radius)
{	
	return !m_Frustrum.SphereIntersection(position, radius);
	return false;// 
}
void CCamera::CalculateInput()
{
}
void CCamera::Move()
{
	UpdateViewMatrix();
	UpdateFrustrum();
}


void CCamera::InvertPitch()
{
	m_Pitch *= -1.0f;
}

const glm::vec3 CCamera::GetDirection() const
{
	float pitch = Utils::ToRadians(m_Pitch);
	float yaw = Utils::ToRadians(m_Yaw);
	float xzLen = cos(pitch);
	glm::vec3 dir;
	dir.z = xzLen * cos(yaw);
	dir.y = sin(pitch);
	dir.x = xzLen * sin(-yaw);
	dir = glm::normalize(dir) *-1.f;
	return dir;
}

const glm::vec3& CCamera::GetPosition() const
{
	return m_Position;
}

const glm::vec3 CCamera::GetRotation() const 
{
	return glm::vec3(m_Pitch, m_Yaw, m_Roll); 
}

const float& CCamera::GetPitch() const
{
	return m_Pitch;
}

const float& CCamera::GetDistance() const
{
	return m_DistanceFromPlayer;
}

void CCamera::SetDistance(float dist)
{
	m_DistanceFromPlayer = dist;
}

void CCamera::SetPitch(float pitch)
{
	m_Pitch = pitch;
}

const float& CCamera::GetYaw() const
{
	return m_Yaw;
}

void CCamera::SetYaw(float yaw)
{
	m_Yaw = yaw;
}

const float& CCamera::GetAngleAround() const
{
	return m_AngleAroundPlayer;
}

void CCamera::SetAngleAround(float a)
{
	m_AngleAroundPlayer = a;
}

const float& CCamera::GetRoll() const
{
	return m_Roll;
}
void CCamera::SetRoll(float roll)
{
	m_Roll = roll;
}

void CCamera::UpdateViewMatrix() 
{
	m_ViewMatrix  = glm::mat4(1.0);
	m_ViewMatrix *= glm::rotate(m_Pitch, 1.0f, 0.0f, 0.0f);
	m_ViewMatrix *= glm::rotate(m_Yaw, 0.0f, 1.0f, 0.0f);
	m_ViewMatrix *= glm::rotate(m_Roll, 0.0f, 0.0f, 1.0f);
	m_ViewMatrix *= glm::translate(-m_Position);
}

const glm::mat4& CCamera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

const glm::vec2 CCamera::GetPositionXZ() const
{
	return glm::vec2(m_Position.x, m_Position.z);
}

