#pragma once
#include <functional>
#include <Utils/IdPool.h>
#include "ICamera.h"

namespace GameEngine
{
class Camera : public ICamera
{
public:
    Camera();
    Camera(float pitch, float yaw);
    Camera(const vec3& position, const vec3& lookAt);

    ~Camera() override = default;

    void Move() override;
    void CalculateInput() override;
    void CalculateZoom(float) override;
    void Lock() override;
    void Unlock() override;
    bool IsLocked() const override;

    void UpdateMatrix() override;

    float GetPitch() const override;
    float GetYaw() const override;

    const Rotation& GetRotation() const override;
    const vec3& GetPosition() const override;
    const vec3& GetDirection() const override;
    const mat4& GetTranslationMatrix() const override;
    const mat4& GetRotationMatrix() const override;
    const mat4& GetViewMatrix() const override;

    void IncreaseYaw(float yaw) override;
    void IncreasePitch(float pitch) override;
    void IncreasePosition(const vec3&) override;
    void IncreasePositionX(float) override;
    void IncreasePositionY(float) override;
    void IncreasePositionZ(float) override;
    void IncreasePositionXZ(const vec2&) override;

    void LookAt(const vec3&) override;
    void SetYaw(float yaw) override;
    void SetPitch(float pitch) override;
    void SetRotation(const Rotation&) override;
    void SetPosition(const vec3&) override;
    IdType SubscribeOnChange(std::function<void(const ICamera&)>) override;
    void UnsubscribeOnChange(IdType) override;

    void NotifySubscribers();

protected:
    void CalculateDirection();
    void UpdateViewMatrix();

protected:
    bool lock_;

private:
    Utils::IdPool idPool_;
    std::vector<std::pair<uint32, std::function<void(const ICamera&)>>> subscribers_;

    vec3 position_;
    Rotation rotation_;
    vec3 direction_;

    mat4 translationMatrix_;
    mat4 rotationMatrix_;
    mat4 viewMatrix_;

private:
    vec3 lastNotifiedPosition_;
    Rotation lastNotifRotation_;
};
}  // namespace GameEngine
