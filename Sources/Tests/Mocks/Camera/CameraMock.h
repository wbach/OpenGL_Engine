#pragma once
#include <gmock/gmock.h>

#include "GameEngine/Camera/ICamera.h"
#include "Types.h"

namespace GameEngine
{
class CameraMock : public ICamera
{
public:
    MOCK_METHOD(void, Update, (), (override));
    MOCK_METHOD(void, UpdateImpl, (), (override));

    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
    MOCK_METHOD(bool, IsLocked, (), (const, override));

    MOCK_METHOD(void, UpdateMatrix, (), (override));

    MOCK_METHOD(float, GetPitch, (), (const, override));
    MOCK_METHOD(float, GetYaw, (), (const, override));

    MOCK_METHOD(const Rotation&, GetRotation, (), (const, override));
    MOCK_METHOD(const vec3&, GetPosition, (), (const, override));
    MOCK_METHOD(const vec3&, GetDirection, (), (const, override));

    MOCK_METHOD(const mat4&, GetViewMatrix, (), (const, override));
    MOCK_METHOD(const mat4&, GetProjectionMatrix, (), (const, override));
    MOCK_METHOD(const mat4&, GetProjectionViewMatrix, (), (const, override));
    MOCK_METHOD(const IProjection&, GetProjection, (), (const, override));

    MOCK_METHOD(void, IncreaseYaw, (float yaw), (override));
    MOCK_METHOD(void, IncreasePitch, (float pitch), (override));
    MOCK_METHOD(void, IncreasePosition, (const vec3&), (override));
    MOCK_METHOD(void, IncreasePositionX, (float), (override));
    MOCK_METHOD(void, IncreasePositionY, (float), (override));
    MOCK_METHOD(void, IncreasePositionZ, (float), (override));
    MOCK_METHOD(void, IncreasePositionXZ, (const vec2&), (override));

    MOCK_METHOD(void, LookAt, (const vec3&), (override));
    MOCK_METHOD(void, SetYaw, (float yaw), (override));
    MOCK_METHOD(void, SetPitch, (float pitch), (override));
    MOCK_METHOD(void, SetRotation, (const Rotation&), (override));
    MOCK_METHOD(void, SetPosition, (const vec3&), (override));
    MOCK_METHOD(void, SetDirection, (const vec3&), (override));

    MOCK_METHOD(IdType, SubscribeOnChange, (std::function<void(const ICamera&)>), (override));
    MOCK_METHOD(void, UnsubscribeOnChange, (IdType), (override));
    MOCK_METHOD(void, SetDirectionAndUp, (const vec3&, const vec3&), (override));
};
}  // namespace GameEngine
