#pragma once
#include <functional>

#include "Types.h"


struct Rotation;

namespace GameEngine
{
struct ICamera
{
    virtual ~ICamera() = default;

    virtual void Update() = 0;
    virtual void UpdateImpl() = 0;

    virtual void Lock()           = 0;
    virtual void Unlock()         = 0;
    virtual bool IsLocked() const = 0;

    virtual void UpdateMatrix() = 0;

    virtual float GetPitch() const = 0;
    virtual float GetYaw() const   = 0;

    virtual const Rotation& GetRotation() const = 0;
    virtual const vec3& GetPosition() const     = 0;
    virtual const vec3& GetDirection() const    = 0;

    virtual const mat4& GetTranslationMatrix() const = 0;
    virtual const mat4& GetRotationMatrix() const    = 0;
    virtual const mat4& GetViewMatrix() const        = 0;

    virtual void IncreaseYaw(float yaw)          = 0;
    virtual void IncreasePitch(float pitch)      = 0;
    virtual void IncreasePosition(const vec3&)   = 0;
    virtual void IncreasePositionX(float)        = 0;
    virtual void IncreasePositionY(float)        = 0;
    virtual void IncreasePositionZ(float)        = 0;
    virtual void IncreasePositionXZ(const vec2&) = 0;

    virtual void LookAt(const vec3&)          = 0;
    virtual void SetYaw(float yaw)            = 0;
    virtual void SetPitch(float pitch)        = 0;
    virtual void SetRotation(const Rotation&) = 0;
    virtual void SetPosition(const vec3&)     = 0;

    virtual IdType SubscribeOnChange(std::function<void(const ICamera&)>) = 0;
    virtual void UnsubscribeOnChange(IdType)                              = 0;
};
}  // namespace GameEngine
