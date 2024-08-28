#pragma once
#include <Common/Transform.h>
#include <GraphicsApi/LineMesh.h>

#include <memory>
#include <optional>
#include <set>
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
using RigidbodyId    = std::optional<IdType>;
using ShapeId        = std::optional<IdType>;
using CollisionSubId = std::optional<IdType>;
using PositionOffset = vec3;
using Scale          = vec3;
using Size           = vec3;
using Radius         = float;

struct CollisionContactInfo;

enum class RigidbodyProperty
{
    NoContactResponse,
    Static
};

using RigidbodyProperties = std::set<RigidbodyProperty>;

struct IPhysicsApi
{
    virtual ~IPhysicsApi()                                                                                            = default;
    virtual void Simulate(float)                                                                                      = 0;
    virtual const GraphicsApi::LineMesh& DebugDraw()                                                                  = 0;
    virtual void EnableSimulation()                                                                                   = 0;
    virtual void DisableSimulation()                                                                                  = 0;
    virtual ShapeId CreateBoxColider(const PositionOffset&, const Scale&, const Size&)                                = 0;
    virtual ShapeId CreateCylinderColider(const PositionOffset&, const Scale&, const Size&)                           = 0;
    virtual ShapeId CreateSphereColider(const PositionOffset&, const Scale&, Radius)                                  = 0;
    virtual ShapeId CreateCapsuleColider(const PositionOffset&, const Scale&, Radius, float height)                   = 0;
    virtual ShapeId CreateTerrainColider(const PositionOffset&, const Scale&, const HeightMap&)                       = 0;
    virtual ShapeId CreateMeshCollider(const PositionOffset&, const std::vector<float>& data, const IndicesVector&, const vec3&,
                                       bool)                                                                          = 0;
    virtual RigidbodyId CreateRigidbody(const ShapeId&, GameObject&, const RigidbodyProperties&, float mass,
                                        bool& isUpdating)                                                             = 0;
    virtual void RemoveRigidBody(const RigidbodyId&)                                                                  = 0;
    virtual void RemoveShape(const ShapeId&)                                                                          = 0;
    virtual void SetVelocityRigidbody(const RigidbodyId&, const vec3&)                                                = 0;
    virtual void ApplyImpulse(const RigidbodyId&, const vec3&)                                                        = 0;
    virtual void IncreaseVelocityRigidbody(const RigidbodyId&, const vec3& velocity)                                  = 0;
    virtual std::optional<vec3> GetVelocity(const RigidbodyId&)                                                       = 0;
    virtual void SetAngularFactor(const RigidbodyId&, float)                                                          = 0;
    virtual void SetAngularFactor(const RigidbodyId&, const vec3&)                                                    = 0;
    virtual std::optional<vec3> GetAngularFactor(const RigidbodyId&)                                                  = 0;
    virtual void SetRotation(const RigidbodyId&, const vec3&)                                                         = 0;
    virtual void SetRotation(const RigidbodyId&, const Quaternion&)                                                   = 0;
    virtual void SetPosition(const RigidbodyId&, const vec3&)                                                         = 0;
    virtual void SetRigidbodyScale(const RigidbodyId&, const vec3&)                                                   = 0;
    virtual void SetShapeScale(const ShapeId&, const vec3&)                                                           = 0;
    virtual std::optional<Quaternion> GetRotation(const RigidbodyId&) const                                           = 0;
    virtual std::optional<common::Transform> GetTransfrom(const RigidbodyId&) const                                   = 0;
    virtual std::optional<RayHit> RayTest(const vec3&, const vec3&) const                                             = 0;
    virtual void enableVisualizatedRigidbody(const RigidbodyId&)                                                      = 0;
    virtual void disableVisualizatedRigidbody(const RigidbodyId&)                                                     = 0;
    virtual void enableVisualizationForAllRigidbodys()                                                                = 0;
    virtual void disableVisualizationForAllRigidbodys()                                                               = 0;
    virtual CollisionSubId setCollisionCallback(const RigidbodyId&, std::function<void(const CollisionContactInfo&)>) = 0;
    virtual void celarCollisionCallback(const CollisionSubId&)                                                        = 0;
};

using IPhysicsApiPtr = std::shared_ptr<IPhysicsApi>;
}  // namespace Physics

}  // namespace GameEngine
