#include "CameraComponent.h"

#include <GLM/GLMUtils.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils/Fsm/Fsm.h>
#include <Utils/TreeNode.h>
#include <Utils/Variant.h>

#include <memory>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection/OrthographicProjection.h"
#include "GameEngine/Renderers/Projection/PerspectiveProjection.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Utils.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_MAIN_CAMERA[]    = "mainCamera";
constexpr char CSTR_NEAR[]           = "near";
constexpr char CSTR_FAR[]            = "far";
constexpr char CSTR_FOV[]            = "fov";
constexpr char CSTR_RENDERING_SIZE[] = "renderingSize";
constexpr char CSTR_SETTINGS[]       = "settings";
}  // namespace

CameraComponent::CameraComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<CameraComponent>(), componentContext, gameObject)
    , settings{Settings::Manual}
    , type{Type::Perspective}
    , renderingSize{EngineConf.renderer.resolution}
    , near{0.3f}
    , far{1000.f}
    , fov{60.f}
    , mainCamera{false}
    , projectionViewMatrix_{1.f}
    , viewMatrix_{1.f}
    , direction_{0.f}
    , lock_{false}
{
}

void CameraComponent::CleanUp()
{
    componentContext_.scene_.GetCameraManager().RemoveCamera(this);
}

void CameraComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, [this]() { awake(); });
}

void CameraComponent::Reload()
{
    if (not IsActive())
        return;

    auto& manager = componentContext_.scene_.GetCameraManager();
    init();

    if (mainCamera)
    {
        manager.SetCameraAsMain(this);
    }
    else
    {
        manager.SetCameraAsMain(nullptr);
    }
}

void CameraComponent::awake()
{
    auto& manager = componentContext_.scene_.GetCameraManager();
    init();
    cameraId = manager.AddCamera(this);
    manager.ActivateCamera(this);

    if (mainCamera)
    {
        manager.SetCameraAsMain(this);
    }
}

void CameraComponent::init()
{
    LOG_DEBUG << "renderingSize: " << renderingSize;

    if (settings == Settings::GlobalConfig)
    {
        renderingSize = EngineConf.renderer.resolution;
        far           = EngineConf.renderer.viewDistance;
        LOG_DEBUG << "GlobalConfig is set. Rendering size and far is used. renderingSize " << renderingSize;
    }
    switch (type)
    {
        case Type::Orthographics:
            projection_ = std::make_unique<OrthographicProjection>(renderingSize, near, far);
            break;
        case Type::Perspective:
            projection_ = std::make_unique<PerspectiveProjection>(renderingSize, near, far, fov);
            break;
    }

    if (settings == Settings::GlobalConfig)
    {
        projection_->SubscribeForGlobalConfigChange();
    }

    UpdateMatrix();
}

void CameraComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& cameraNode, GameObject& gameObject)
    {
        auto component = std::make_unique<CameraComponent>(componentContext, gameObject);

        ::Read(cameraNode.getChild(CSTR_MAIN_CAMERA), component->mainCamera);
        ::Read(cameraNode.getChild(CSTR_RENDERING_SIZE), component->renderingSize);
        ::Read(cameraNode.getChild(CSTR_NEAR), component->near);
        ::Read(cameraNode.getChild(CSTR_FAR), component->far);
        ::Read(cameraNode.getChild(CSTR_FOV), component->fov);

        if (auto node = cameraNode.getChild(CSTR_TYPE))
        {
            if (auto type = magic_enum::enum_cast<Type>(node->value_))
            {
                component->type = *type;
            }
        }
        if (auto node = cameraNode.getChild(CSTR_SETTINGS))
        {
            if (auto settings = magic_enum::enum_cast<Settings>(node->value_))
            {
                component->settings = *settings;
            }
        }
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<CameraComponent>(), func);
}

void CameraComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});

    ::write(node.addChild(CSTR_MAIN_CAMERA), mainCamera);
    ::write(node.addChild(CSTR_RENDERING_SIZE), renderingSize);
    ::write(node.addChild(CSTR_NEAR), near);
    ::write(node.addChild(CSTR_FAR), far);
    ::write(node.addChild(CSTR_FOV), fov);
    ::write(node.addChild(CSTR_TYPE), magic_enum::enum_name(type));
    ::write(node.addChild(CSTR_SETTINGS), magic_enum::enum_name(settings));
}
const mat4& CameraComponent::GetProjectionViewMatrix() const
{
    return projectionViewMatrix_;
}
const mat4& CameraComponent::GetViewMatrix() const
{
    return viewMatrix_;
}
const mat4& CameraComponent::GetProjectionMatrix() const
{
    return projection_->GetMatrix();
}
void CameraComponent::UpdateMatrix()
{
    UpdateViewMatrix();
    if (projection_)
    {
        projectionViewMatrix_ = projection_->GetMatrix() * viewMatrix_;
    }
}

void CameraComponent::UpdateViewMatrix()
{
    const auto& worldTransform = thisObject_.GetWorldTransform();
    auto rotationMatrix        = glm::mat4_cast(worldTransform.GetRotation().value_);
    auto translationMatrix     = glm::translate(-worldTransform.GetPosition());

    viewMatrix_ = rotationMatrix * translationMatrix;
    CalculateDirection();
}

void CameraComponent::CalculateDirection()
{
    const auto& worldTransform = thisObject_.GetWorldTransform();
    direction_                 = glm::normalize(VECTOR_BACKWARD * worldTransform.GetRotation().value_);
}
void CameraComponent::Lock()
{
    lock_ = true;
}
void CameraComponent::Unlock()
{
    lock_ = false;
}
bool CameraComponent::IsLocked() const
{
    return lock_;
}
void CameraComponent::SetPosition(const vec3& position)
{
    thisObject_.SetWorldPosition(position);
}
void CameraComponent::LookAt(const vec3& lookAtPosition)
{
    thisObject_.SetWorldRotation(Utils::lookAt(lookAtPosition, thisObject_.GetWorldTransform().GetPosition()));
    CalculateDirection();
    UpdateMatrix();
}
float CameraComponent::GetPitch() const
{
    return thisObject_.GetWorldTransform().GetRotation().GetEulerDegrees()->x;
}
void CameraComponent::SetPitch(float angle)
{
    glm::quat pitch = glm::angleAxis(glm::radians(angle), vec3(1.f, 0.f, 0.f));
    glm::quat yaw   = glm::angleAxis(glm::yaw(thisObject_.GetWorldTransform().GetRotation().value_), vec3(0.f, 1.f, 0.f));
    thisObject_.SetWorldRotation(glm::normalize(pitch * yaw));
    CalculateDirection();
}
void CameraComponent::SetRotation(const Rotation& rotation)
{
    thisObject_.SetWorldRotation(rotation);
}
float CameraComponent::GetYaw() const
{
    return thisObject_.GetWorldTransform().GetRotation().GetEulerDegrees()->y;
}
void CameraComponent::SetYaw(float angle)
{
    glm::quat pitch = glm::angleAxis(glm::pitch(thisObject_.GetWorldTransform().GetRotation().value_), vec3(1.f, 0.f, 0.f));
    glm::quat yaw   = glm::angleAxis(glm::radians(angle), vec3(0.f, 1.f, 0.f));
    thisObject_.SetWorldRotation(glm::normalize(pitch * yaw));
    CalculateDirection();
}
void CameraComponent::IncreaseYaw(float yaw)
{
    auto rotation = thisObject_.GetWorldTransform().GetRotation().value_;
    rotation *= glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));
    thisObject_.SetWorldRotation(rotation);
    CalculateDirection();
}
void CameraComponent::IncreasePitch(float pitch)
{
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    auto rotation    = glm::normalize(qPitch * thisObject_.GetWorldTransform().GetRotation().value_);
    thisObject_.SetWorldRotation(rotation);
    CalculateDirection();
}
void CameraComponent::IncreasePosition(const vec3& v)
{
    auto position = thisObject_.GetWorldTransform().GetPosition();
    position.x += v.x;
    position.y += v.y;
    position.z += v.z;
    thisObject_.SetWorldPosition(position);
}
void CameraComponent::IncreasePositionX(float x)
{
    auto position = thisObject_.GetWorldTransform().GetPosition();
    position.x += x;
    thisObject_.SetWorldPosition(position);
}
void CameraComponent::IncreasePositionY(float y)
{
    auto position = thisObject_.GetWorldTransform().GetPosition();
    position.y += y;
    thisObject_.SetWorldPosition(position);
}
void CameraComponent::IncreasePositionZ(float z)
{
    auto position = thisObject_.GetWorldTransform().GetPosition();
    position.z += z;
    thisObject_.SetWorldPosition(position);
}
void CameraComponent::IncreasePositionXZ(const vec2& v)
{
    auto position = thisObject_.GetWorldTransform().GetPosition();
    position.x += v.x;
    position.z += v.y;
    thisObject_.SetWorldPosition(position);
}
void CameraComponent::UnsubscribeOnChange(IdType id)
{
    thisObject_.UnsubscribeOnLocalTransfromChange(id);
}
IdType CameraComponent::SubscribeOnChange(std::function<void(const ICamera&)> callback)
{
    return thisObject_.SubscribeOnWorldTransfomChange([this, callback](const auto&) { callback(*this); });
}
const vec3& CameraComponent::GetPosition() const
{
    return thisObject_.GetWorldTransform().GetPosition();
}
const Rotation& CameraComponent::GetRotation() const
{
    return thisObject_.GetWorldTransform().GetRotation();
}
void CameraComponent::UpdateImpl()
{
    if (not lock_)
        Update();
}
void CameraComponent::Update()
{
}
const vec3& CameraComponent::GetDirection() const
{
    return direction_;
}
const IProjection& CameraComponent::GetProjection() const
{
    if (not projection_)
    {
        LOG_ERROR << "Projection not set!";
    }
    return *projection_;
}
void CameraComponent::Activate()
{
    if (not projection_ or not cameraId)
    {
        LOG_DEBUG << "Camera not init";
        return;
    }
    auto& manager = componentContext_.scene_.GetCameraManager();

    bool isCameraActiveAlready = manager.GetActiveCamera(*cameraId) != nullptr;
    if (isCameraActiveAlready)
    {
        LOG_DEBUG << "Camera is already activated";
        return;
    }

    manager.ActivateCamera(this);
    BaseComponent::Activate();
}
void CameraComponent::Deactivate()
{
    LOG_DEBUG << "Try deactivate";
    auto& manager = componentContext_.scene_.GetCameraManager();
    manager.DeactivateCamera(this);
    BaseComponent::Deactivate();
}
void CameraComponent::SetActive(bool v)
{
    if (v)
    {
        Activate();
    }
    else
    {
        Deactivate();
    }
}
}  // namespace Components
}  // namespace GameEngine
