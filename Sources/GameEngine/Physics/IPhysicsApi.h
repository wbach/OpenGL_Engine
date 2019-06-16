#pragma once
#include <memory>
#include <vector>
#include "Common/Transform.h"
#include "ShapeType.h"
#include "Types.h"

namespace GameEngine
{
namespace Physics
{
struct IPhysicsApi
{
    virtual ~IPhysicsApi()
    {
    }
    virtual void Simulate()                                                       = 0;
    virtual void DebugDraw(const mat4& viewMatrix, const mat4& projectionMatrix)  = 0;
    virtual void EnableSimulation()                                               = 0;
    virtual void DisableSimulation()                                              = 0;
    virtual void SetSimulationStep(float step)                                    = 0;
    virtual uint32 CreateBoxColider(const vec3& positionOffset, const vec3& size) = 0;
    virtual uint32 CreateSphereColider(const vec3& positionOffset, float radius)  = 0;
    // virtual void CreateMeshColider(vec3 positionOffset, std::vect) = 0;
    virtual uint32 CreateTerrainColider(const vec3& positionOffset, const vec2ui& size, const std::vector<float>& data,
                                        const vec3& scale)                                        = 0;
    virtual uint32 CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data,
                                      const IndicesVector& indicies, float scaleFactor)           = 0;
    virtual uint32 CreateRigidbody(uint32 shapeId, common::Transform&, float mass, bool isStatic) = 0;
    virtual void SetVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity)                   = 0;
    virtual void RemoveRigidBody(uint32 id)                                                       = 0;
};

using IPhysicsApiPtr = std::shared_ptr<IPhysicsApi>;
}  // namespace Physics

}  // namespace GameEngine
