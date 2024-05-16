#include "CollisionResultCallback.h"

#include "../CollisionContactInfo.h"
#include "Converter.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{

CollisionResultCallback::CollisionResultCallback(std::function<void(const CollisionContactInfo &)> c)
    : callback(c)
{
}

btScalar CollisionResultCallback::addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap,
                                                  int partId0, int index0, const btCollisionObjectWrapper *colObj1Wrap,
                                                  int partId1, int index1)

{
    DEBUG_LOG("on collision");
    CollisionContactInfo collisionContactInfo{
        .pos1         = Convert(cp.getPositionWorldOnA()),
        .pos2         = Convert(cp.getPositionWorldOnB()),
        .rigidbodyId1 = static_cast<uint32>(colObj0Wrap->getCollisionObject()->getUserIndex()),
        .rigidbodyId2 = static_cast<uint32>(colObj1Wrap->getCollisionObject()->getUserIndex())};

    callback(collisionContactInfo);
    return 0;
}

}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
