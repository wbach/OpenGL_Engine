#pragma once
#include "ICamera.h"
#include "Frustrum.h"
#include "Mutex.hpp"
#include <atomic>

namespace GameEngine
{
	class BaseCamera : public ICamera
	{
	public:
		BaseCamera();
		BaseCamera(float pitch, float yaw);
		virtual ~BaseCamera() {}
		virtual void UpdateMatrix() override;
		virtual vec3 GetPosition()	override;
		virtual void SetPosition(const vec3&) override;
		virtual const vec3	GetDirection()	const override;
		virtual const vec3	GetRotation()	const override;
		virtual const vec2	GetPositionXZ() const override;
		virtual const mat4& GetViewMatrix() const override;
		virtual bool CheckCulling(const vec3& position, float radius) override;

		virtual void Move() override {};
		virtual void CalculateInput() override {};
		virtual void CalculateZoom(float zoom_lvl) override {};
		virtual void InvertPitch() override;

		virtual float GetPitch() override;
		virtual float GetYaw()	 override;
		virtual float GetRoll()  override;

		virtual void SetPitch(float pitch) override;
		virtual void SetRoll(float roll)   override;
		virtual void SetYaw(float yaw)     override;

	protected:
		void UpdateFrustrum();
		void UpdateViewMatrix();

	protected:
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
} // GameEngine
