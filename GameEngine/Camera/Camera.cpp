#include "Camera.h"
#include "Utils.h"
#include "GLM/GLMUtils.h"

CCamera::CCamera() 
: distanceFromPlayer(99)
, angleAroundPlayer(0)
, pitch(20)
, yaw(0)
, roll(0)
, position(0)
{	
	UpdateViewMatrix();
	UpdateFrustrum();
}

void CCamera::SetProjectionMatrix(const mat4 & projection_matrix)
{
    projectionMatrix = projection_matrix;
}

void CCamera::SetPosition(vec3 position)
{
    position = position;
}
void CCamera::UpdateFrustrum()
{
    frustrum.CalculatePlanes(projectionMatrix * viewMatrix);
}
bool CCamera::CheckFrustrumSphereCulling(const vec3 & position, const float& radius)
{	
    return !frustrum.SphereIntersection(position, radius);
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
    pitch *= -1.0f;
}

const vec3 CCamera::GetDirection() const
{
    float pitch_ = Utils::ToRadians(pitch);
    float yaw_ = Utils::ToRadians(yaw);
    float xzLen = cos(pitch_);
	vec3 dir;
    dir.z = xzLen * cos(yaw_);
    dir.y = sin(pitch_);
    dir.x = xzLen * sin(-yaw_);
	dir = glm::normalize(dir) *-1.f;
	return dir;
}

const vec3& CCamera::GetPosition() const
{
    return position;
}

const vec3 CCamera::GetRotation() const 
{
    return vec3(pitch, yaw, roll);
}

const float& CCamera::GetPitch() const
{
    return pitch;
}

const float& CCamera::GetDistance() const
{
	return distanceFromPlayer;
}

void CCamera::SetDistance(float dist)
{
	distanceFromPlayer = dist;
}

void CCamera::SetPitch(float pitch)
{
    pitch = pitch;
}

const float& CCamera::GetYaw() const
{
    return yaw;
}

void CCamera::SetYaw(float yaw)
{
    yaw = yaw;
}

const float& CCamera::GetAngleAround() const
{
    return angleAroundPlayer;
}

void CCamera::SetAngleAround(float a)
{
    angleAroundPlayer = a;
}

const float& CCamera::GetRoll() const
{
    return roll;
}
void CCamera::SetRoll(float roll)
{
    roll = roll;
}

void CCamera::UpdateViewMatrix() 
{
    viewMatrix  = mat4(1.0);
    viewMatrix *= glm::rotate(pitch, 1.0f, 0.0f, 0.0f);
    viewMatrix *= glm::rotate(yaw, 0.0f, 1.0f, 0.0f);
    viewMatrix *= glm::rotate(roll, 0.0f, 0.0f, 1.0f);
    viewMatrix *= glm::translate(-position);
}

const mat4& CCamera::GetViewMatrix() const
{
    return viewMatrix;
}

const vec2 CCamera::GetPositionXZ() const
{
    return vec2(position.x, position.z);
}

