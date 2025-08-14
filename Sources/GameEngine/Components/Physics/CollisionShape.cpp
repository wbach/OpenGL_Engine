#include "CollisionShape.h"

#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
CollisionShape::CollisionShape(size_t type, ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , collisionShapeId_(0)
    , positionOffset(0.f)
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
    if (collisionShapeId_)
    {
        componentContext_.physicsApi_.RemoveShape(*collisionShapeId_);
        collisionShapeId_ = std::nullopt;
    }
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
