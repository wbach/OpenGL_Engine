#pragma once
#include <LinearMath/btMotionState.h>

#include <atomic>

namespace GameEngine
{
class GameObject;

namespace Physics
{
namespace Bullet
{
class BulletMotionState : public btMotionState
{
public:
    BulletMotionState(GameObject&, const btVector3&, std::atomic_bool&);
    void getWorldTransform(btTransform&) const override;
    void setWorldTransform(const btTransform&) override;

private:
    GameObject& go;
    btVector3 offset;
    std::atomic_bool& isSynchronizing;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
