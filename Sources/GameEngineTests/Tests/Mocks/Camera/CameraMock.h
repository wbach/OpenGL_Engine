#pragma once
#include "Types.h"
#include "GameEngine/Camera/Camera.h"
#include <gmock/gmock.h>

namespace GameEngine
{
	struct CameraMock : public ICamera
	{
		MOCK_METHOD0(Move, void());
		MOCK_METHOD0(CalculateInput, void());
		MOCK_METHOD1(CalculateZoom, void(float));

		MOCK_METHOD0(UpdateMatrix, void());
		MOCK_METHOD0(InvertPitch, void());

		MOCK_METHOD0(GetPitch, float());
		MOCK_METHOD0(GetYaw, float());
		MOCK_METHOD0(GetRoll, float());

		MOCK_METHOD1(SetPitch, void(float));
		MOCK_METHOD1(SetRoll, void(float));
		MOCK_METHOD1(SetYaw, void(float));

		MOCK_CONST_METHOD0(GetPositionXZ, const vec2());
		MOCK_CONST_METHOD0(GetRotation, const vec3());
		MOCK_CONST_METHOD0(GetDirection, const vec3());
		MOCK_METHOD0(GetPosition, vec3());
		MOCK_CONST_METHOD0(GetViewMatrix, const mat4&());
		MOCK_METHOD1(SetPosition, void(const vec3&));

		MOCK_METHOD2(CheckCulling, bool(const vec3&, float));
	};

} // GameEngine
