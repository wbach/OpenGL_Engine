#pragma once
#include "ICamera.h"

namespace GameEngine
{
class BaseCamera : public ICamera
{
public:
    BaseCamera();
    BaseCamera(float pitch, float yaw);
    BaseCamera(const vec3& position, const vec3& lookAt);

    virtual ~BaseCamera()
    {
    }

    virtual void Move() override;
    virtual void CalculateInput() override;
    virtual void CalculateZoom(float zoom_lvl) override;

    virtual void UpdateMatrix() override;
    virtual void InvertPitch() override;

    virtual float GetPitch() const override;
    virtual float GetYaw() const override;
    virtual float GetRoll() const override;

    virtual const vec3& GetRotation() const override;
    virtual const vec3& GetPosition() const override;
    virtual const vec3& GetDirection() const override;
    virtual const mat4& GetViewMatrix() const override;

    virtual void LookAt(const vec3&) override;
    virtual void SetYaw(float yaw) override;
    virtual void SetRoll(float roll) override;
    virtual void SetPitch(float pitch) override;
    virtual void SetRotation(const vec3&) override;
    virtual void SetPosition(const vec3&) override;

protected:
    void CalculateDirection();
    void UpdateViewMatrix();

protected:
    vec3 up_;
    vec3 position_;
    vec3 rotation_;
    vec3 direction_;
    mat4 viewMatrix_;
};
}  // namespace GameEngine
