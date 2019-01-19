#include "CollisionShape.h"

#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
CollisionShape::CollisionShape(ComponentsType type, const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , collisionShapeId_(0)
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
const vec3 & CollisionShape::GetPositionOffset() const
{
    return positionOffset_;
}
}  // namespace Components
}  // namespace GameEngine
