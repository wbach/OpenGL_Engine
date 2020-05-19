#pragma once
#include <Common/Transform.h>
#include <GraphicsApi/LineMesh.h>

#include <memory>
#include <optional>
#include <vector>

#include "ShapeType.h"
#include "Types.h"

namespace GameEngine
{
class HeightMap;
namespace Physics
{
struct IPhysicsApi
{
    virtual ~IPhysicsApi()                                                                      = default;
    virtual void Simulate()                                                                     = 0;
    virtual const GraphicsApi::LineMesh& DebugDraw()                                            = 0;
    virtual void EnableSimulation()                                                             = 0;
    virtual void DisableSimulation()                                                            = 0;
    virtual void SetSimulationStep(float step)                                                  = 0;
    virtual uint32 CreateBoxColider(const vec3& positionOffset, const vec3& size)               = 0;
    virtual uint32 CreateSphereColider(const vec3& positionOffset, float radius)                = 0;
    virtual uint32 CreateCapsuleColider(const vec3& positionOffset, float radius, float height) = 0;
    // virtual void CreateMeshColider(vec3 positionOffset, std::vect) = 0;
    virtual uint32 CreateTerrainColider(const vec3& positionOffset, const HeightMap&, const vec3& scale) = 0;
    virtual uint32 CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data,
                                      const IndicesVector& indicies, float scaleFactor)                  = 0;
    virtual uint32 CreateRigidbody(uint32 shapeId, common::Transform&, float mass, bool isStatic)        = 0;
    virtual void SetVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity)                          = 0;
    virtual void ApplyImpulse(uint32 rigidBodyId, const vec3& impulse)                                   = 0;
    virtual void IncreaseVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity)                     = 0;
    virtual std::optional<vec3> GetVelocity(uint32 rigidBodyId)                                          = 0;
    virtual void SetAngularFactor(uint32 rigidBodyId, float value)                                       = 0;
    virtual void SetAngularFactor(uint32 rigidBodyId, const vec3& value)                                 = 0;
    virtual std::optional<vec3> GetAngularFactor(uint32 rigidBodyId)                                     = 0;
    virtual void RemoveRigidBody(uint32 id)                                                              = 0;
    virtual void SetRotation(uint32 rigidBodyId, const vec3&)                                            = 0;
    virtual void SetRotation(uint32 rigidBodyId, const Quaternion&)                                      = 0;
    virtual void SetPosition(uint32 rigidBodyId, const vec3&)                                            = 0;
    virtual std::optional<Quaternion> GetRotation(uint32 rigidBodyId) const                              = 0;
    virtual std::optional<common::Transform> GetTransfrom(uint32 rigidBodyId) const                      = 0;
};

using IPhysicsApiPtr = std::shared_ptr<IPhysicsApi>;
}  // namespace Physics

}  // namespace GameEngine
