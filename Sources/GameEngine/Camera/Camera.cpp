#include "Camera.h"
#include "Utils.h"
#include "GLM/GLMUtils.h"

CCamera::CCamera() 
: CCamera(20, 0)
{	

}

CCamera::CCamera(float pitch, float yaw)
	: pitch(pitch)
	, yaw(yaw)
    , distanceFromPlayer(99)
    , angleAroundPlayer(0)
    , roll(0)
    , position(0)
{
    UpdateViewMatrix();
    UpdateFrustrum();
}

void CCamera::UpdateMatrix()
{
	UpdateViewMatrix();
	UpdateFrustrum();
}

void CCamera::SetProjectionMatrix(const mat4 & projection_matrix)
{
    projectionMatrix = projection_matrix;
}

void CCamera::SetPosition(vec3 p)
{
	std::lock_guard<std::mutex> l(pmutex);
    position = p;
}
void CCamera::UpdateFrustrum()
{
	std::lock_guard<std::mutex> l(viewMatrixMutex);
    frustrum.CalculatePlanes(projectionMatrix * viewMatrix);
}
bool CCamera::CheckFrustrumSphereCulling(const vec3 & position, const float& radius)
{	
    return !frustrum.SphereIntersection(position, radius);
}
void CCamera::CalculateInput()
{
}
void CCamera::InvertPitch()
{
	pitch.store(pitch.load() - 1.0f);
}

const vec3 CCamera::GetDirection() const
{
    float pitch_ = Utils::ToRadians(pitch.load());
    float yaw_ = Utils::ToRadians(yaw.load());
    float xzLen = cos(pitch_);
	vec3 dir;
    dir.z = xzLen * cos(yaw_);
    dir.y = sin(pitch_);
    dir.x = xzLen * sin(-yaw_);
	dir = glm::normalize(dir) *-1.f;
	return dir;
}

const vec3& CCamera::GetPosition()
{
	std::lock_guard<std::mutex> l(pmutex);
    return position;
}

const vec3 CCamera::GetRotation() const 
{
    return vec3(pitch, yaw, roll);
}

float CCamera::GetDistance() const
{
	return distanceFromPlayer;
}

void CCamera::SetDistance(float dist)
{
	distanceFromPlayer = dist;
}

float CCamera::GetPitch()
{
    return pitch.load();
}

void CCamera::SetPitch(float p)
{
	pitch.store(p);
}

float CCamera::GetYaw()
{
    return yaw.load();
}

void CCamera::SetYaw(float y)
{
	yaw.store(y);
}

float CCamera::GetAngleAround() const
{
    return angleAroundPlayer;
}

void CCamera::SetAngleAround(float a)
{
    angleAroundPlayer = a;
}

float CCamera::GetRoll()
{
    return roll.load();
}
void CCamera::SetRoll(float r)
{
	roll.store(r);
}

void CCamera::UpdateViewMatrix() 
{
	float p = GetPitch();
	float y = GetYaw();
	float r = GetRoll();
	auto pos = GetPosition();

	viewMatrix = mat4(1.0f);
	viewMatrix *= glm::rotate(p, 1.0f, 0.0f, 0.0f);
	viewMatrix *= glm::rotate(y, 0.0f, 1.0f, 0.0f);
	viewMatrix *= glm::rotate(r, 0.0f, 0.0f, 1.0f);
	viewMatrix *= glm::translate(-pos);
}

const mat4& CCamera::GetViewMatrix()
{
    return viewMatrix;
}

const vec2 CCamera::GetPositionXZ() const
{
    return vec2(position.x, position.z);
}
