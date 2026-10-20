#include "BulletMotionState.h"

#include "Converter.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
BulletMotionState::BulletMotionState(GameObject& go, const btVector3& offset, std::atomic_bool& isSynchronizing)
    : go{go}
    , offset(offset)
    , isSynchronizing{isSynchronizing}
{
}
void BulletMotionState::getWorldTransform(btTransform& worldTrans) const
{
    btTransform graphicTrans;
    graphicTrans.setOrigin(Convert(go.GetWorldTransform().GetPosition()));
    graphicTrans.setRotation(Convert(go.GetWorldTransform().GetRotation().value_));
    worldTrans = graphicTrans * btTransform(btQuaternion::getIdentity(), offset);
}
void BulletMotionState::setWorldTransform(const btTransform& worldTrans)
{
    isSynchronizing   = true;
    auto graphicTrans = worldTrans * btTransform(btQuaternion::getIdentity(), offset).inverse();
    go.SetWorldPositionRotation(Convert(graphicTrans.getOrigin()), Convert(graphicTrans.getRotation()));
    isSynchronizing = false;
}
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
