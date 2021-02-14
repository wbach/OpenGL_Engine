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
    , positionOffset_(0.f)
{
}
CollisionShape::~CollisionShape()
{
    DEBUG_LOG("GameObject Id : " + std::to_string(thisObject_.GetId()) + " collisionShapeId_ = " + (collisionShapeId_ ? std::to_string(*collisionShapeId_) : std::string("std::nullopt")));
}
void CollisionShape::setScale(const vec3& scale)
{
    DEBUG_LOG(std::to_string(scale));
    if (collisionShapeId_)
        componentContext_.physicsApi_.SetShapeScale(*collisionShapeId_, scale);
}
std::optional<uint32> CollisionShape::GetCollisionShapeId() const
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
