#pragma once
#include <memory>
#include <btBulletDynamicsCommon.h>

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
struct Shape
{
    Shape() = default;
    Shape(std::unique_ptr<btCollisionShape> btShape)
        : btShape_(std::move(btShape))
        , positionOffset_(0.f, 0.f, 0.f)
        , dynamicShapeAllowed_{true}
    {
    }
    Shape(std::unique_ptr<btCollisionShape> btShape, const btVector3& offset)
        : btShape_(std::move(btShape))
        , positionOffset_(offset)
        , dynamicShapeAllowed_{true}
    {
    }

    virtual ~Shape() = default;

    std::unique_ptr<btCollisionShape> btShape_{nullptr};
    btVector3 positionOffset_{0.f, 0.f, 0.f};
    bool dynamicShapeAllowed_{true};
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
