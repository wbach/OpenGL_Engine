#pragma once
#include <Types.h>
#include <btBulletDynamicsCommon.h>

#include <memory>

namespace GameEngine
{
class GameObject;

namespace Physics
{
namespace Bullet
{
struct Rigidbody
{
    std::unique_ptr<btRigidBody> btRigidbody_;
    GameObject& gameObject;
    btVector3& positionOffset_;
    bool& isUpdating_;

    IdType shapeId{0};
    bool shapeOwner{false};
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
