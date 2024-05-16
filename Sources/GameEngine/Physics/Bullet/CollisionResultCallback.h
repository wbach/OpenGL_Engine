#pragma once
#include <btBulletDynamicsCommon.h>

#include <functional>

namespace GameEngine
{
namespace Physics
{
struct CollisionContactInfo;

namespace Bullet
{
struct CollisionResultCallback : public btCollisionWorld::ContactResultCallback
{
public:
    CollisionResultCallback(std::function<void(const CollisionContactInfo&)>);
    btScalar addSingleResult(btManifoldPoint&, const btCollisionObjectWrapper*, int, int,
                             const btCollisionObjectWrapper*, int, int) override;

private:
    std::function<void(const CollisionContactInfo&)> callback;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
