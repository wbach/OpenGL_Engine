#pragma once
#include <Rotation.h>
#include <Utils/IdPool.h>

#include <functional>
#include <memory>

#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "ICamera.h"
#include "Types.h"

namespace GameEngine
{
class IProjection;
class Camera : public ICamera
{
public:
    Camera();
    Camera(float pitch, float yaw);
    Camera(const vec3& position, const vec3& lookAt);
    Camera(std::unique_ptr<IProjection>, float pitch, float yaw);
    Camera(std::unique_ptr<IProjection>, const vec3& position, const vec3& lookAt);

    ~Camera() override = default;

    void UpdateImpl() override;
    void Update() override;
    void Lock() override;
    void Unlock() override;
    bool IsLocked() const override;

    void UpdateMatrix() override;

    float GetPitch() const override;
    float GetYaw() const override;

    const Rotation& GetRotation() const override;
    const vec3& GetPosition() const override;
    const vec3& GetDirection() const override;
    const mat4& GetViewMatrix() const override;
    const mat4& GetProjectionMatrix() const override;
    const mat4& GetProjectionViewMatrix() const override;
    const IProjection& GetProjection() const override;

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
    void SetDirection(const vec3&) override;

    IdType SubscribeOnChange(std::function<void(const ICamera&)>) override;
    void UnsubscribeOnChange(IdType) override;

    void SetDirectionAndUp(const vec3& dir, const vec3& up) override;

    void NotifySubscribers();

    glm::vec3 GetForward() const;

    void UpdatePerFrameBuffer() override;
    const PerFrameBuffer& GetPerFrameBuffer() const override;

protected:
    void CalculateDirection();
    void UpdateViewMatrix();

protected:
    bool lock_;

protected:
    Utils::IdPool idPool_;
    std::vector<std::pair<uint32, std::function<void(const ICamera&)>>> subscribers_;

    vec3 position_;
    Rotation rotation_;
    vec3 direction_;

    std::unique_ptr<IProjection> projection_;
    mat4 projectionViewMatrix_;
    mat4 viewMatrix_;

    mat4 translationMatrix_;
    mat4 rotationMatrix_;

    PerFrameBuffer perFrameBuffer;

private:
    vec3 lastNotifiedPosition_;
    Rotation lastNotifRotation_;
};
}  // namespace GameEngine
