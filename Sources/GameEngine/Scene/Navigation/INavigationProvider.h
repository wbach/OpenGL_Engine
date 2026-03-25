#pragma once
#include <Types.h>

#include <vector>

#include "EngineApi.h"

namespace GameEngine
{
namespace Physics
{
class IPhysicsApi;
}

class Model;
class BoundingBox;
class TerrainHeightGetter;
class ENGINE_API INavigationProvider
{
public:
    virtual ~INavigationProvider() = default;

    virtual std::vector<vec3> CalculatePath(const vec3&, const vec3&)                       = 0;
    virtual void BakeTerrain(const TerrainHeightGetter&, float)                             = 0;
    virtual void AddObstacle(const BoundingBox&)                                            = 0;
    virtual void AddObstacle(Model&, const mat4&, float = 0.f)                              = 0;
    virtual void AddPhysicsObstacle(Physics::IPhysicsApi&, const BoundingBox&, float = 0.f) = 0;
};
}  // namespace GameEngine
