#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Camera/Camera.h"
#include "Types.h"

namespace GameEngine
{
class CameraMock : public ICamera
{
public:
    MOCK_METHOD0(Move, void());
    MOCK_METHOD0(CalculateInput, void());
    MOCK_METHOD1(CalculateZoom, void(float zoom_lvl));
    MOCK_METHOD0(Lock, void());
    MOCK_METHOD0(Unlock, void());
    MOCK_METHOD0(UpdateMatrix, void());
    MOCK_METHOD0(InvertPitch, void());
    MOCK_CONST_METHOD0(GetPitch, float());
    MOCK_CONST_METHOD0(GetYaw, float());
    MOCK_CONST_METHOD0(GetRoll, float());
    MOCK_CONST_METHOD0(GetRotation, const vec3&());
    MOCK_CONST_METHOD0(GetPosition, const vec3&());
    MOCK_CONST_METHOD0(GetDirection, const vec3&());
    MOCK_CONST_METHOD0(GetViewMatrix, const mat4&());
    MOCK_METHOD1(IncreaseYaw, float());
    MOCK_METHOD1(IncreasePitch, float());
    MOCK_METHOD1(IncreasePosition, const vec3&());
    MOCK_METHOD1(IncreasePositionX, float());
    MOCK_METHOD1(IncreasePositionY, float());
    MOCK_METHOD1(IncreasePositionZ, float());
    MOCK_METHOD1(IncreasePosition, const vec2&());
    MOCK_METHOD1(LookAt, void(const vec3&));
    MOCK_METHOD1(SetYaw, void(float yaw));
    MOCK_METHOD1(SetRoll, void(float roll));
    MOCK_METHOD1(SetPitch, void(float pitch));
    MOCK_METHOD1(SetRotation, void(const vec3&));
    MOCK_METHOD1(SetPosition, void(const vec3&));
};
}  // namespace GameEngine
