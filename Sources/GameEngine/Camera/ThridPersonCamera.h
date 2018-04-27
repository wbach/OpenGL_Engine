#pragma once
#include "Camera.h"
#include "Clock.hpp"
#include "Mutex.hpp"
#include <list>

namespace common
{
	class Transform;
} // common

namespace GameEngine
{
	class InputManager;


	enum CameraState
	{
		MOVING,
		ROTATE_PITCH,
		ROTATE_YAW,
		SHAKING
	};

	template<class T>
	struct CameraEvent
	{
		T startValue;
		T moveValue;
		float startTime;
		float endTime;
	};

	class ThirdPersonCamera : public BaseCamera
	{
	public:
		ThirdPersonCamera(InputManager* input_manager, common::Transform* lookAt);
		virtual ~ThirdPersonCamera() override;
		void Move() override;
		void CalculateInput() override;
		void CalculateZoom(float zoom_lvl) override;
		void SetLookAtTransform(common::Transform* lookAt);

	private:
		void StaticCameraMove();
		void SmoothCameraMove();
		void LockCamera();
		void SetCaptureMouse(bool capture);
		void LockPitch();
		void LockYaw();
		void CalculateCameraPosition(float horizontal_distance, float vertical_distance);
		float CalculateHorizontalDistance();
		float CalculateVerticalDistance();
		void CalculateYaw();
		vec2 CalcualteMouseMove();
		void CalculatePitch(const vec2& d_move);
		void CalculateAngleAroundPlayer(const vec2& d_move);
		bool IsOnDestinationPos();
		bool IsOnDestinationPitch();
		bool IsOnDestinationYaw();

		float GetTime() const;
		bool FindState(CameraState state);

		template<class T>
		T CalculateNewValueInTimeInterval(const CameraEvent<T>& t, float time) const;

		template<class T>
		T ProcessState(CameraEvent<T>& stateInfo, const T& destination, float time, bool& remove);

		template<class T>
		void ControlState(CameraEvent<T>& stateInfo, CameraState state, const T& startValue, const T& destination, float time, bool use);

	private:
		InputManager* inputManager;
		common::Transform* lookAt_;

		vec3 lookAtPosition_;
		vec3 lookAtRotataion_;

		bool	isShowCursor;
		vec3	offset;
		float	mousevel;
		bool	captureMouse;

		float destinationYaw;
		float destinationPitch;

		float moveTime;
		vec3 destinationPosition;

		CameraEvent<vec3> moveStateInfo_;

		Timepoint referenceTime;
		std::list<CameraState> states_;

		float distanceFromPlayer;
		float angleAroundPlayer;

		Utils::CClock clock;
	};
} // GameEngine
