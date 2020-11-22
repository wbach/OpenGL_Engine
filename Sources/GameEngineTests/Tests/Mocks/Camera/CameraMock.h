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
    MOCK_CONST_METHOD0(IsLocked, bool());
    MOCK_METHOD0(UpdateMatrix, void());
    MOCK_CONST_METHOD0(GetPitch, float());
    MOCK_CONST_METHOD0(GetYaw, float());
    MOCK_CONST_METHOD0(GetRotation, const Rotation&());
    MOCK_CONST_METHOD0(GetPosition, const vec3&());
    MOCK_CONST_METHOD0(GetDirection, const vec3&());
    MOCK_CONST_METHOD0(GetTranslationMatrix, const mat4&());
    MOCK_CONST_METHOD0(GetRotationMatrix, const mat4&());
    MOCK_CONST_METHOD0(GetViewMatrix, const mat4&());
    MOCK_METHOD1(IncreaseYaw, void(float));
    MOCK_METHOD1(IncreasePitch, void(float));
    MOCK_METHOD1(IncreasePosition, void(const vec3&));
    MOCK_METHOD1(IncreasePositionX, void(float));
    MOCK_METHOD1(IncreasePositionY, void(float));
    MOCK_METHOD1(IncreasePositionZ, void(float));
    MOCK_METHOD1(IncreasePositionXZ, void(const vec2&));
    MOCK_METHOD1(LookAt, void(const vec3&));
    MOCK_METHOD1(SetYaw, void(float yaw));
    MOCK_METHOD1(SetPitch, void(float pitch));
    MOCK_METHOD1(SetRotation, void(const Rotation&));
    MOCK_METHOD1(SetPosition, void(const vec3&));
    MOCK_METHOD1(SubscribeOnChange, uint32(std::function<void(const ICamera&)>));
    MOCK_METHOD1(UnsubscribeOnChange, void(uint32));
};
}  // namespace GameEngine
