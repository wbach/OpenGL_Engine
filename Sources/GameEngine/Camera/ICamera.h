#pragma once
#include "Types.h"

namespace GameEngine
{
	struct ICamera
	{
		virtual ~ICamera() {}

		virtual void Move() = 0;
		virtual void CalculateInput() = 0;
		virtual void CalculateZoom(float zoom_lvl) = 0;

		virtual void UpdateMatrix() = 0;
		virtual void InvertPitch() = 0;

		virtual float GetPitch() = 0;
		virtual float GetYaw() = 0;
		virtual float GetRoll() = 0;

		virtual void SetPitch(float pitch) = 0;
		virtual void SetRoll(float roll) = 0;
		virtual void SetYaw(float yaw) = 0;

		virtual const vec2	GetPositionXZ() const = 0;
		virtual const vec3	GetRotation()	const = 0;
		virtual const vec3	GetDirection()	const = 0;
		virtual const vec3& GetPosition() = 0;
		virtual const mat4& GetViewMatrix() const = 0;
		virtual void SetPosition(const vec3&) = 0;

		virtual bool CheckCulling(const vec3& position, float radius) = 0;
	};
} // GameEngine
