#pragma once

#include <memory>
#include <vector>
#include <optional>

#include "GameEngine/Physics/Bullet/Shape.h"
#include "GameEngine/Physics/Bullet/Converter.h"
#include "GameEngine/Physics/PhysicsApiTypes.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

#include <btBulletDynamicsCommon.h>

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{

class ShapeFactory
{
public:
    static std::unique_ptr<Shape> CreateBox(const PositionOffset& positionOffset, const Scale& scale, const Size& size);
    static std::unique_ptr<Shape> CreateCylinder(const PositionOffset& positionOffset, const Scale& scale, const Size& size);
    static std::unique_ptr<Shape> CreateSphere(const PositionOffset& positionOffset, const Scale&, Radius radius);
    static std::unique_ptr<Shape> CreateCapsule(const PositionOffset& positionOffset, const Scale& scale, Radius radius, float height);
    static std::unique_ptr<Shape> CreateTerrain(const PositionOffset& positionOffset, const Scale& scale, const HeightMap& heightMap);
    static std::optional<std::unique_ptr<Shape>> CreateMesh(const PositionOffset& positionOffset, 
                                                              const std::vector<float>& data, 
                                                              const IndicesVector& indicies, 
                                                              const vec3& scale, 
                                                              bool autoOptimize);
};

} // namespace Bullet
} // namespace Physics
} // namespace GameEngine
