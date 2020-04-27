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
    , rigidbody_(nullptr)
    , camera_(camera)
    , projection_(projection)
    , cameraStartPos_(camera_.GetPosition())
{
    rigidbody_ = gameObject.GetComponent<Components::Rigidbody>();

    mouseZcoord_ = CalculateMouseZCoord(gameObject_.GetTransform().GetPosition());
    offset_ = gameObject_.GetTransform().GetPosition() - GetMouseAsWorldPoint(input_.GetMousePosition(), mouseZcoord_);
}
void DragObject::Update()
{
    auto mouseWorldPoint = GetMouseAsWorldPoint(input_.GetMousePosition(), mouseZcoord_);
    auto newPosition     = mouseWorldPoint + offset_;

    auto cameraPosChange = camera_.GetPosition() - cameraStartPos_;
    newPosition          = newPosition + cameraPosChange;

    gameObject_.GetTransform().SetPosition(newPosition);

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
    vec4 clipCoords(point.x, point.y, -1.0f, 1.0f);
    auto eyeCoords   = glm::inverse(projection_.GetProjectionMatrix()) * clipCoords;
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
}  // namespace GameEngine
