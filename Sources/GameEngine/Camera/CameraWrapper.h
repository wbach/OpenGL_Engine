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
    ICamera* Get() const;

    void Update();

    void Move();
    void CalculateInput();
    void CalculateZoom(float zoom_lvl);
    bool IsLocked() const;
    void Lock();
    void Unlock();

    void UpdateMatrix();

    float GetPitch() const;
    float GetYaw() const;

    const Rotation& GetRotation() const;
    const vec3& GetPosition() const;
    const vec3& GetDirection() const;
    const mat4& GetViewMatrix() const;

    void LookAt(const vec3&);
    void SetYaw(float yaw);
    void SetPitch(float pitch);
    void SetRotation(const Rotation&);
    void SetPosition(const vec3&);

    uint32 SubscribeOnChange(std::function<void(const ICamera&)>);
    void UnsubscribeOnChange(uint32);

private:
    BaseCamera baseCamera_;
    ICamera* camera_;
};
}  // namespace GameEngine
