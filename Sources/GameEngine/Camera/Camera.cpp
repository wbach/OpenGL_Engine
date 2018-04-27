#include "Camera.h"
#include "Utils.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
	BaseCamera::BaseCamera()
		: BaseCamera(20, 0)
	{
	}
	BaseCamera::BaseCamera(float pitch, float yaw)
		: pitch(pitch)
		, yaw(yaw)
		, roll(0)
		, position(0)
	{
		UpdateMatrix();
	}
	void BaseCamera::UpdateMatrix()
	{
		UpdateViewMatrix();
		UpdateFrustrum();
	}
	void BaseCamera::SetPosition(const vec3& p)
	{
		std::lock_guard<std::mutex> l(pmutex);
		position = p;
	}
	void BaseCamera::UpdateFrustrum()
	{
		std::lock_guard<std::mutex> l(viewMatrixMutex);
		frustrum.CalculatePlanes(projectionMatrix * viewMatrix);
	}
	bool BaseCamera::CheckCulling(const vec3 & position, float radius)
	{
		return !frustrum.SphereIntersection(position, radius);
	}
	void BaseCamera::InvertPitch()
	{
		pitch.store(pitch.load() - 1.0f);
	}
	const vec3 BaseCamera::GetDirection() const
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
	const vec3& BaseCamera::GetPosition()
	{
		std::lock_guard<std::mutex> l(pmutex);
		return position;
	}
	const vec3 BaseCamera::GetRotation() const
	{
		return vec3(pitch, yaw, roll);
	}
	float BaseCamera::GetPitch()
	{
		return pitch.load();
	}
	void BaseCamera::SetPitch(float p)
	{
		pitch.store(p);
	}
	float BaseCamera::GetYaw()
	{
		return yaw.load();
	}
	void BaseCamera::SetYaw(float y)
	{
		yaw.store(y);
	}
	float BaseCamera::GetRoll()
	{
		return roll.load();
	}
	void BaseCamera::SetRoll(float r)
	{
		roll.store(r);
	}
	void BaseCamera::UpdateViewMatrix()
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
	const mat4& BaseCamera::GetViewMatrix() const
	{
		return viewMatrix;
	}
	const vec2 BaseCamera::GetPositionXZ() const
	{
		return vec2(position.x, position.z);
	}
} // GameEngine
