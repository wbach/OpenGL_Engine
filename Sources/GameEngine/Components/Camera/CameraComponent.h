#pragma once
#include <Utils/IdPool.h>
#include <Utils/Rotation.h>

#include <optional>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "Types.h"

#ifndef USE_GNU
//#ifndef USE_MINGW 
#include <Utils/UndifineWindows.h>
//#endif
#endif


namespace GameEngine
{
class ICamera;
class IProjection;

namespace Components
{
class CameraComponent : public BaseComponent, public ICamera
{
public:
    enum Type
    {
        Orthographics,
        Perspective
    };
    enum Settings
    {
        Manual,
        GlobalConfig
    };

    Settings settings;
    Type type;

    vec2ui renderingSize;
    float near;
    float far;
    float fov;
    bool mainCamera;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_ENUM(settings)
        FIELD_ENUM(type)
        FIELD_BOOL(mainCamera)
        FIELD_VECTOR2I(renderingSize)
        FIELD_FLOAT(near)
        FIELD_FLOAT(far)
        FIELD_FLOAT(fov)
    END_FIELDS()
    // clang-format on

public:
    CameraComponent(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    const Rotation& GetRotation() const override;
    const vec3& GetPosition() const override;
    void UpdateMatrix() override;
    const mat4& GetProjectionViewMatrix() const override;
    const mat4& GetViewMatrix() const override;
    const mat4& GetProjectionMatrix() const override;

    void UpdateImpl() override;
    void Update() override;
    void Lock() override;
    void Unlock() override;
    bool IsLocked() const override;

    float GetPitch() const override;
    float GetYaw() const override;

    const vec3& GetDirection() const override;

    const IProjection& GetProjection() const override;

    void IncreaseYaw(float) override;
    void IncreasePitch(float) override;
    void IncreasePosition(const vec3&) override;
    void IncreasePositionX(float) override;
    void IncreasePositionY(float) override;
    void IncreasePositionZ(float) override;
    void IncreasePositionXZ(const vec2&) override;

    void LookAt(const vec3&) override;
    void SetYaw(float) override;
    void SetPitch(float) override;
    void SetRotation(const Rotation&) override;
    void SetPosition(const vec3&) override;

    void SetDirectionAndUp(const vec3& dir, const vec3& up) override;

    IdType SubscribeOnChange(std::function<void(const ICamera&)>) override;
    void UnsubscribeOnChange(IdType) override;

private:
    void awake();
    void init();
    void UpdateViewMatrix();
    void CalculateDirection();
    void Activate() override;
    void Deactivate() override;
    void SetActive(bool) override;

private:
    Utils::IdPool idPool_;
    std::vector<std::pair<uint32, std::function<void(const ICamera&)>>> subscribers_;

    std::unique_ptr<IProjection> projection_;

    mat4 projectionViewMatrix_;
    mat4 viewMatrix_;

    vec3 direction_;

    bool lock_;

    std::optional<IdType> cameraId;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
