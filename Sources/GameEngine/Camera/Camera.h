#pragma once
#include <functional>
#include <Common/Rotation.h>
#include "ICamera.h"

namespace GameEngine
{
class BaseCamera : public ICamera
{
public:
    BaseCamera();
    BaseCamera(float pitch, float yaw);
    BaseCamera(const vec3& position, const vec3& lookAt);

    virtual ~BaseCamera() override = default;

    virtual void Move() override;
    virtual void CalculateInput() override;
    virtual void CalculateZoom(float) override;
    virtual void Lock() override;
    virtual void Unlock() override;
    virtual bool IsLocked() const override;

    virtual void UpdateMatrix() override;

    virtual float GetPitch() const override;
    virtual float GetYaw() const override;

    virtual const vec3& GetRotation() const override;
    virtual const vec3& GetPosition() const override;
    virtual const vec3& GetDirection() const override;
    virtual const mat4& GetViewMatrix() const override;

    virtual void IncreaseYaw(float yaw) override;
    virtual void IncreasePitch(float pitch) override;
    virtual void IncreasePosition(const vec3&) override;
    virtual void IncreasePositionX(float) override;
    virtual void IncreasePositionY(float) override;
    virtual void IncreasePositionZ(float) override;
    virtual void IncreasePositionXZ(const vec2&) override;

    virtual void LookAt(const vec3&) override;
    virtual void SetYaw(float yaw) override;
    virtual void SetPitch(float pitch) override;
    virtual void SetRotation(const vec3&) override;
    virtual void SetPosition(const vec3&) override;

    void NotifySubscribers();
    uint32 SubscribeOnChange(std::function<void(const ICamera&)>) override;
    void UnsubscribeOnChange(uint32) override;

protected:
    void CalculateDirection();
    void UpdateViewMatrix();

protected:
    bool lock_;
    common::Rotation rotation_;

private:
    uint32 idPool_;
    std::vector<std::pair<uint32, std::function<void(const ICamera&)>>> subscribers_;

    vec3 up_;
    vec3 position_;
    vec3 direction_;
    mat4 viewMatrix_;

private:
    vec3 lastNotifiedPosition_;
    common::Rotation lastNotifRotation_;
};
}  // namespace GameEngine
