#include "DragObject.h"

#include <Common/Transform.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Renderers/Projection.h"

namespace GameEngine
{
DragObject::DragObject(Input::InputManager& manager, GameObject& gameObject, const CameraWrapper& camera,
                       const Projection& projection)
    : input_(manager)
    , gameObject_(gameObject)
    , camera_(camera)
    , projection_(projection)
    , rigidbody_(nullptr)
{
    mouseZcoord_ = CalculateMouseZCoord(gameObject_.worldTransform.GetPosition());
    DEBUG_LOG(std::to_string(mouseZcoord_));
    offset_ = gameObject_.worldTransform.GetPosition() - GetMouseAsWorldPoint(input_.GetMousePosition(), mouseZcoord_);
    rigidbody_ = gameObject.GetComponent<Components::Rigidbody>();
}
void DragObject::Update()
{
    auto mouseWorldPoint = GetMouseAsWorldPoint(input_.GetMousePosition(), mouseZcoord_);
    auto newPosition     = mouseWorldPoint + offset_;

    gameObject_.worldTransform.SetPosition(newPosition);

    if (rigidbody_)
    {
        rigidbody_->SetPosition(newPosition);
    }
}
vec3 DragObject::WorldToScreenPoint(const vec3& point)
{
    return Utils::Vec4ToVec3(projection_.GetProjectionMatrix() * camera_.GetViewMatrix() * vec4(point, 1.f));
}
vec3 DragObject::ScreenToWorldPoint(const vec3& point)
{
    auto eyeCoords   = glm::inverse(projection_.GetProjectionMatrix()) * vec4(point, 1.f);
    auto worldCoords = glm::inverse(camera_.GetViewMatrix()) * eyeCoords;
    return Utils::Vec4ToVec3(worldCoords);
}
float DragObject::CalculateMouseZCoord(const vec3& objectPosition)
{
    return WorldToScreenPoint(objectPosition).z;
}
vec3 DragObject::GetMouseAsWorldPoint(const vec2& mousePosition, float zCoord)
{
    return ScreenToWorldPoint(vec3(mousePosition, zCoord));
}
}  // namespace GameEngine
