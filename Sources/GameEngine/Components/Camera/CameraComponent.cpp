#include "CameraComponent.h"

#include <GLM/GLMUtils.h>
#include <Input/InputManager.h>
#include <Utils/Fsm/Fsm.h>
#include <Utils/TreeNode.h>
#include <Utils/Variant.h>
#include <memory>

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/Fsm/ThridPersonCameraEvents.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection/PerspectiveProjection.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "glm/glm.hpp"

namespace GameEngine
{
namespace Components
{
using namespace Camera;

CameraComponent::CameraComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<CameraComponent>(), componentContext, gameObject)
{
}

void CameraComponent::CleanUp()
{
    // componentContext_.scene_.GetCameraManager().RemoveCamera(&fsmContext->camera);
}

void CameraComponent::ReqisterFunctions()
{
    //RegisterFunction(FunctionType::Awake, [this]() { awake(); });
}

void CameraComponent::Reload()
{
}

void CameraComponent::awake()
{
    projection_ = std::make_unique<PerspectiveProjection>();
    //componentContext_.scene_.GetCameraManager().AddCamera(*this); TO DO: add without ownership
    UpdateMatrix();
}

void CameraComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode&, GameObject& gameObject)
    { return std::make_unique<CameraComponent>(componentContext, gameObject); };

    regsiterComponentReadFunction(GetComponentType<CameraComponent>(), func);
}

void CameraComponent::write(TreeNode& node) const
{
    const std::string CSTR_TYPE = "type";
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
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
    projectionViewMatrix_ = projection_->GetMatrix() * viewMatrix_;
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
}  // namespace Components
}  // namespace GameEngine
