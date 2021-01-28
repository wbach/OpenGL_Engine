#include "CollisionShape.h"

#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
CollisionShape::CollisionShape(size_t type, ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , collisionShapeId_(0)
    , positionOffset_(0.f)
{
}
uint32 CollisionShape::GetCollisionShapeId() const
{
    return collisionShapeId_;
}
void CollisionShape::SetPostionOffset(const vec3& position)
{
    positionOffset_ = position;
}
const vec3& CollisionShape::GetPositionOffset() const
{
    return positionOffset_;
}
void CollisionShape::registerReadFunctions()
{
}
void CollisionShape::write(TreeNode&) const
{
}
}  // namespace Components
}  // namespace GameEngine
