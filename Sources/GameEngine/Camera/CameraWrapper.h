#pragma once
#include <memory>
#include "Camera.h"

namespace GameEngine
{
class CameraWrapper
{
public:
    CameraWrapper();
    CameraWrapper(ICamera& camera);
    void Set(ICamera& camera);

    void Update();

    void Move();
    void CalculateInput();
    void CalculateZoom(float zoom_lvl);
    void Lock();
    void Unlock();

    void UpdateMatrix();
    void InvertPitch();

    float GetPitch() const;
    float GetYaw() const;
    float GetRoll() const;

    const vec3& GetRotation() const;
    const vec3& GetPosition() const;
    const vec3& GetDirection() const;
    const mat4& GetViewMatrix() const;

    void LookAt(const vec3&);
    void SetYaw(float yaw);
    void SetRoll(float roll);
    void SetPitch(float pitch);
    void SetRotation(const vec3&);
    void SetPosition(const vec3&);

    uint32 SubscribeOnChange(std::function<void(const ICamera&)>);
    void UnsubscribeOnChange(uint32);

private:
    BaseCamera baseCamera_;
    ICamera* camera_;
};
}  // namespace GameEngine
