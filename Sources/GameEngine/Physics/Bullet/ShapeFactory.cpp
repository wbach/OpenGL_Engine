#include "ShapeFactory.h"

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <Utils/Variant.h>
#include <btBulletDynamicsCommon.h>


#include <memory>
#include <optional>
#include <vector>

#include "GameEngine/Physics/Bullet/Converter.h"
#include "GameEngine/Physics/Bullet/MeshShape.h"
#include "GameEngine/Physics/Bullet/Shape.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{

std::unique_ptr<Shape> ShapeFactory::CreateBox(const PositionOffset& positionOffset, const Scale& scale, const Size& size)
{
    return std::make_unique<Shape>(std::make_unique<btBoxShape>(Convert(size)),
                                   Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)));
}

std::unique_ptr<Shape> ShapeFactory::CreateCylinder(const PositionOffset& positionOffset, const Scale& scale, const Size& size)
{
    return std::make_unique<Shape>(std::make_unique<btCylinderShape>(Convert(size)),
                                   Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)));
}

std::unique_ptr<Shape> ShapeFactory::CreateSphere(const PositionOffset& positionOffset, const Scale&, Radius radius)
{
    return std::make_unique<Shape>(std::make_unique<btSphereShape>(radius), Convert(positionOffset));
}

std::unique_ptr<Shape> ShapeFactory::CreateCapsule(const PositionOffset& positionOffset, const Scale& scale, Radius radius,
                                                   float height)
{
    return std::make_unique<Shape>(std::make_unique<btCapsuleShape>(radius, height / 2.f),
                                   Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)));
}

std::unique_ptr<Shape> ShapeFactory::CreateTerrain(const PositionOffset& positionOffset, const Scale& scale,
                                                   const HeightMap& heightMap)
{
    auto shape = std::make_unique<Shape>();

    auto width  = static_cast<int>(heightMap.GetImage().width);
    auto height = static_cast<int>(heightMap.GetImage().height);
    std::visit(
        visitor{
            [&](const std::vector<uint8>& data)
            {
                shape->btShape_.reset(new btHeightfieldTerrainShape(width, height, &data[0], 1.f, heightMap.GetMinimumHeight(),
                                                                    heightMap.GetMaximumHeight(), 1, PHY_UCHAR, false));
            },
            [&](const std::vector<float>& data)
            {
                shape->btShape_.reset(new btHeightfieldTerrainShape(width, height, &data[0], 1.f, heightMap.GetMinimumHeight(),
                                                                    heightMap.GetMaximumHeight(), 1, PHY_FLOAT, false));
            },
            [](std::monostate) { LOG_ERROR << "Height map data is not set!."; },
        },
        heightMap.GetImage().getImageData());

    float scaleX = scale.x / static_cast<float>(heightMap.GetImage().width - 1);
    float scaleY = scale.z / static_cast<float>(heightMap.GetImage().height - 1);

    shape->btShape_->setLocalScaling(btVector3(scaleX, scale.y, scaleY));

    auto offset            = heightMap.GetMaximumHeight() * scale.y - (heightMap.GetDeltaHeight() * scale.y / 2.f);
    shape->positionOffset_ = Convert(positionOffset + vec3(0, offset, 0));
    shape->isTerrainShape  = true;
    return shape;
}

std::optional<std::unique_ptr<Shape>> ShapeFactory::CreateMesh(const PositionOffset& positionOffset,
                                                               const std::vector<float>& data, const IndicesVector& indicies,
                                                               const vec3& scale, bool autoOptimize)
{
    if (data.empty())
    {
        return std::nullopt;
    }

    if (not autoOptimize)
    {
        if (indicies.empty())
            return std::nullopt;

        auto btMesh = std::make_unique<btTriangleMesh>(true, false);

        for (size_t i = 0; i < indicies.size(); i += 3)
        {
            if (i + 2 < indicies.size())
            {
                auto idx0 = static_cast<size_t>(indicies[i]);
                auto idx1 = static_cast<size_t>(indicies[i + 1]);
                auto idx2 = static_cast<size_t>(indicies[i + 2]);
                if (3 * idx0 + 2 < data.size() and 3 * idx1 + 2 < data.size() and 3 * idx2 + 2 < data.size())
                {
                    btVector3 v0(data[3 * idx0], data[3 * idx0 + 1], data[3 * idx0 + 2]);
                    btVector3 v1(data[3 * idx1], data[3 * idx1 + 1], data[3 * idx1 + 2]);
                    btVector3 v2(data[3 * idx2], data[3 * idx2 + 1], data[3 * idx2 + 2]);

                    btMesh->addTriangle(v0, v1, v2);
                }
            }
        }

        auto meshShape      = std::make_unique<MeshShape>(std::move(btMesh));
        auto btMeshShapePtr = meshShape->btMesh_.get();
        meshShape->btShape_ = std::make_unique<btBvhTriangleMeshShape>(btMeshShapePtr, true, true);
        meshShape->btShape_->setLocalScaling(Convert(scale));
        meshShape->positionOffset_ = Convert(positionOffset);

        return meshShape;
    }
    else
    {
        auto hullShape = std::make_unique<btConvexHullShape>();

        for (size_t i = 0; i < data.size(); i += 3)
        {
            if (i + 2 < data.size())
                hullShape->addPoint(btVector3(data[i], data[i + 1], data[i + 2]), false);
        }

        hullShape->optimizeConvexHull();
        hullShape->setLocalScaling(Convert(scale));
        hullShape->recalcLocalAabb();

        auto shape = std::make_unique<Shape>(std::move(hullShape), Convert(positionOffset));
        return shape;
    }
}

}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
