#pragma once
#include <functional>
#include "Types.h"

namespace GameEngine
{
struct ICamera
{
    virtual ~ICamera() = default;

    virtual void Move()                        = 0;
    virtual void CalculateInput()              = 0;
    virtual void CalculateZoom(float zoom_lvl) = 0;
    virtual void Lock()                        = 0;
    virtual void Unlock()                      = 0;

    virtual void UpdateMatrix() = 0;
    virtual void InvertPitch()  = 0;

    virtual float GetPitch() const = 0;
    virtual float GetYaw() const   = 0;
    virtual float GetRoll() const  = 0;

    virtual const vec3& GetRotation() const   = 0;
    virtual const vec3& GetPosition() const   = 0;
    virtual const vec3& GetDirection() const  = 0;
    virtual const mat4& GetViewMatrix() const = 0;

    virtual void IncreaseYaw(float yaw)          = 0;
    virtual void IncreasePitch(float pitch)      = 0;
    virtual void IncreasePosition(const vec3&)   = 0;
    virtual void IncreasePositionX(float)        = 0;
    virtual void IncreasePositionY(float)        = 0;
    virtual void IncreasePositionZ(float)        = 0;
    virtual void IncreasePositionXZ(const vec2&) = 0;

    virtual void LookAt(const vec3&)      = 0;
    virtual void SetYaw(float yaw)        = 0;
    virtual void SetRoll(float roll)      = 0;
    virtual void SetPitch(float pitch)    = 0;
    virtual void SetRotation(const vec3&) = 0;
    virtual void SetPosition(const vec3&) = 0;

    virtual uint32 SubscribeOnChange(std::function<void(const ICamera&)>) = 0;
    virtual void UnsubscribeOnChange(uint32)                              = 0;
};
}  // namespace GameEngine
