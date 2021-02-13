#pragma once
#include <Common/Transform.h>
#include <GraphicsApi/LineMesh.h>

#include <memory>
#include <optional>
#include <vector>

#include "GameEngine/Objects/GameObject.h"
#include "RayHit.h"
#include "ShapeType.h"
#include "Types.h"

namespace GameEngine
{
class HeightMap;
namespace Physics
{
using RigidbodyId    = uint32;
using ShapeId        = uint32;
using PositionOffset = vec3;

struct IPhysicsApi
{
    virtual ~IPhysicsApi()                                                                                    = default;
    virtual void Simulate()                                                                                   = 0;
    virtual const GraphicsApi::LineMesh& DebugDraw()                                                          = 0;
    virtual void EnableSimulation()                                                                           = 0;
    virtual void DisableSimulation()                                                                          = 0;
    virtual void SetSimulationStep(float step)                                                                = 0;
    virtual uint32 CreateBoxColider(const PositionOffset&, const vec3& scale, const vec3& size)               = 0;
    virtual uint32 CreateSphereColider(const PositionOffset&, const vec3& scale, float radius)                = 0;
    virtual uint32 CreateCapsuleColider(const PositionOffset&, const vec3& scale, float radius, float height) = 0;
    virtual uint32 CreateTerrainColider(const PositionOffset&, const vec3& scale, const HeightMap&)           = 0;
    virtual uint32 CreateMeshCollider(const PositionOffset&, const std::vector<float>& data,
                                      const IndicesVector& indicies, const vec3&, bool)                       = 0;
    virtual uint32 CreateRigidbody(ShapeId, GameObject&, float mass, bool isStatic, bool& isUpdating)         = 0;
    virtual void RemoveRigidBody(ShapeId)                                                                     = 0;
    virtual void RemoveShape(ShapeId)                                                                         = 0;
    virtual void SetVelocityRigidbody(RigidbodyId, const vec3&)                                               = 0;
    virtual void ApplyImpulse(RigidbodyId, const vec3&)                                                       = 0;
    virtual void IncreaseVelocityRigidbody(RigidbodyId, const vec3& velocity)                                 = 0;
    virtual std::optional<vec3> GetVelocity(RigidbodyId)                                                      = 0;
    virtual void SetAngularFactor(RigidbodyId, float)                                                         = 0;
    virtual void SetAngularFactor(RigidbodyId, const vec3&)                                                   = 0;
    virtual std::optional<vec3> GetAngularFactor(RigidbodyId)                                                 = 0;
    virtual void SetRotation(RigidbodyId, const vec3&)                                                        = 0;
    virtual void SetRotation(RigidbodyId, const Quaternion&)                                                  = 0;
    virtual void SetPosition(RigidbodyId, const vec3&)                                                        = 0;
    virtual void SetRigidbodyScale(RigidbodyId, const vec3&)                                                  = 0;
    virtual void SetShapeScale(ShapeId, const vec3&)                                                          = 0;
    virtual std::optional<Quaternion> GetRotation(RigidbodyId) const                                          = 0;
    virtual std::optional<common::Transform> GetTransfrom(RigidbodyId) const                                  = 0;
    virtual std::optional<RayHit> RayTest(const vec3&, const vec3&) const                                     = 0;
    virtual void setVisualizatedRigidbody(RigidbodyId)                                                        = 0;
    virtual void enableVisualizationForAllRigidbodys()                                                        = 0;
    virtual void disableVisualizationForAllRigidbodys()                                                       = 0;
};

using IPhysicsApiPtr = std::shared_ptr<IPhysicsApi>;
}  // namespace Physics

}  // namespace GameEngine
