#include "DragObject.h"

#include <Common/Transform.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
DragObject::DragObject(Input::InputManager& manager, GameObject& gameObject, const ICamera& camera)
    : input_(manager)
    , gameObject_(gameObject)
    , rigidbody_(nullptr)
    , camera_(camera)
    , cameraStartPos_(camera_.GetPosition())
{
    startupTransform_ = gameObject.GetLocalTransform();
    rigidbody_ = gameObject.GetComponent<Components::Rigidbody>();

    mouseZcoord_ = CalculateMouseZCoord(gameObject_.GetWorldTransform().GetPosition());
    offset_      = gameObject_.GetWorldTransform().GetPosition() - GetMouseAsWorldPoint(input_.GetMousePosition(), mouseZcoord_);
}
DragObject::~DragObject()
{
}
void DragObject::Update()
{
    auto mouseWorldPoint = GetMouseAsWorldPoint(input_.GetMousePosition(), mouseZcoord_);
    auto newPosition     = mouseWorldPoint + offset_;

    auto cameraPosChange = camera_.GetPosition() - cameraStartPos_;
    newPosition          = newPosition + cameraPosChange;

    KeyDirectionLock(newPosition);

    gameObject_.SetWorldPosition(newPosition);

    if (rigidbody_)
    {
        rigidbody_->SetPosition(newPosition);
    }
}
vec3 DragObject::WorldToScreenPoint(const vec3& point)
{
    return Utils::Vec4ToVec3(camera_.GetProjectionViewMatrix() * vec4(point, 1.f));
}
vec3 DragObject::ScreenToWorldPoint(const vec3& point)
{
    vec4 clipCoords(point.x, point.y, -1.0f, 1.0f);
    auto eyeCoords   = glm::inverse(camera_.GetProjectionMatrix()) * clipCoords;
    auto coords      = vec4(eyeCoords.x, eyeCoords.y, -1.f, 0.0f);
    auto worldCoords = glm::inverse(camera_.GetViewMatrix()) * coords;
    return Utils::Vec4ToVec3(worldCoords) * point.z;
}
float DragObject::CalculateMouseZCoord(const vec3& objectPosition)
{
    return WorldToScreenPoint(objectPosition).z;
}
vec3 DragObject::GetMouseAsWorldPoint(const vec2& mousePosition, float zCoord)
{
    return ScreenToWorldPoint(vec3(mousePosition, zCoord));
}
void DragObject::KeyDirectionLock(vec3& newPosition)
{
    const auto& position = gameObject_.GetWorldTransform().GetPosition();

    if (input_.GetKey(KeyCodes::B))
    {
        newPosition.y = position.y;
        newPosition.z = position.z;
    }
    if (input_.GetKey(KeyCodes::N))
    {
        newPosition.x = position.x;
        newPosition.z = position.z;
    }
    if (input_.GetKey(KeyCodes::M))
    {
        newPosition.x = position.x;
        newPosition.y = position.y;
    }
}
const common::Transform& DragObject::GetObjectStartupTransfrom() const
{
    return startupTransform_;
}
GameObject& DragObject::GetGameObject()
{
    return gameObject_;
}
}  // namespace GameEngine
