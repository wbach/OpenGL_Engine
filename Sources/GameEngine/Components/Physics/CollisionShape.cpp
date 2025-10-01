#include "CollisionShape.h"

#include <Logger/Log.h>
#include <optional>

#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
CollisionShape::CollisionShape(const ComponentType& componentType, ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(componentType, componentContext, gameObject)
    , collisionShapeId_(std::nullopt)
    , positionOffset(0.f)
    , connectedRigidbody{nullptr}
{
}
void CollisionShape::setScale(const vec3& scale)
{
    if (collisionShapeId_)
        componentContext_.physicsApi_.SetShapeScale(*collisionShapeId_, scale);
}
const Physics::ShapeId& CollisionShape::GetCollisionShapeId() const
{
    return collisionShapeId_;
}
void CollisionShape::CleanUp()
{
    if (connectedRigidbody)
    {
        connectedRigidbody->CleanUp();
    }
    else
    {
        LOG_DEBUG << "CollisionShape::CleanUp: not connected to rigidbody";
        RemoveShape();
    }
}

void CollisionShape::RemoveShape()
{
    if (collisionShapeId_)
    {
        componentContext_.physicsApi_.RemoveShape(*collisionShapeId_);
        collisionShapeId_.reset();
    }
}

void CollisionShape::Reload()
{
    LOG_DEBUG << thisObject_.GetName();
    if (connectedRigidbody)
    {
        connectedRigidbody->RemoveRigidbody();
        RemoveShape();
        InitShape();
        connectedRigidbody->CreateRigidbody();
    }
}
void CollisionShape::Connect(Rigidbody& rigidbody)
{
    LOG_DEBUG << "Connecting to rigidbody " << thisObject_.GetName();
    connectedRigidbody = &rigidbody;
}

void CollisionShape::Disconnect()
{
    LOG_DEBUG << "Disconnecting from rigidbody" << thisObject_.GetName();
    connectedRigidbody = nullptr;
}
void CollisionShape::SetPostionOffset(const vec3& position)
{
    positionOffset = position;
}
const vec3& CollisionShape::GetPositionOffset() const
{
    return positionOffset;
}
void CollisionShape::registerReadFunctions()
{
}
void CollisionShape::write(TreeNode&) const
{
}
}  // namespace Components
}  // namespace GameEngine
