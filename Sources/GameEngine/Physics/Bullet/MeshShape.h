#pragma once
#include "Shape.h"

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
struct MeshShape : public Shape
{
    MeshShape() = default;
    MeshShape(std::unique_ptr<btTriangleMesh> btMesh)
        : btMesh_(std::move(btMesh))
    {
        dynamicShapeAllowed_ = false;
    }

    ~MeshShape()
    {
        btShape_.reset();
        btMesh_.reset();
    }
    std::unique_ptr<btTriangleMesh> btMesh_;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
