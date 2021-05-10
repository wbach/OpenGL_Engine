#pragma once
#include <Utils/IdPool.h>
#include <btBulletDynamicsCommon.h>

#include <memory>

#include "DebugDrawer.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "Rigidbody.h"
#include "Shape.h"

namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
class BulletAdapter : public IPhysicsApi
{
public:
    BulletAdapter();
    ~BulletAdapter() override;

    void Simulate() override;
    const GraphicsApi::LineMesh& DebugDraw() override;
    void SetSimulationStep(float) override;
    void EnableSimulation() override;
    void DisableSimulation() override;
    ShapeId CreateBoxColider(const PositionOffset&, const Scale&, const Size&) override;
    ShapeId CreateSphereColider(const PositionOffset&, const Scale&, Radius) override;
    ShapeId CreateCapsuleColider(const PositionOffset&, const Scale&, Radius, float height) override;
    ShapeId CreateTerrainColider(const PositionOffset&, const Scale&, const HeightMap& heightMap) override;
    ShapeId CreateMeshCollider(const PositionOffset&, const std::vector<float>& data, const IndicesVector& indicies,
                               const vec3&, bool) override;
    RigidbodyId CreateRigidbody(const ShapeId&, GameObject&, float, bool, bool&) override;
    void RemoveRigidBody(const RigidbodyId&) override;
    void RemoveShape(const ShapeId&) override;
    void SetVelocityRigidbody(const RigidbodyId&, const vec3& velocity) override;
    void ApplyImpulse(const RigidbodyId&, const vec3& impulse) override;
    void IncreaseVelocityRigidbody(const RigidbodyId&, const vec3&) override;
    std::optional<vec3> GetVelocity(const RigidbodyId&) override;
    void SetAngularFactor(const RigidbodyId&, float) override;
    void SetAngularFactor(const RigidbodyId&, const vec3&) override;
    std::optional<vec3> GetAngularFactor(const RigidbodyId&) override;
    void SetRotation(const RigidbodyId&, const vec3&) override;
    void SetRotation(const RigidbodyId&, const Quaternion&) override;
    void SetPosition(const RigidbodyId&, const vec3&) override;
    void SetRigidbodyScale(const RigidbodyId&, const vec3&) override;
    void SetShapeScale(const ShapeId&, const vec3&) override;
    std::optional<Quaternion> GetRotation(const RigidbodyId&) const override;
    std::optional<common::Transform> GetTransfrom(const RigidbodyId&) const override;
    std::optional<RayHit> RayTest(const vec3&, const vec3&) const override;
    void setVisualizatedRigidbody(const RigidbodyId&) override;
    void enableVisualizationForAllRigidbodys() override;
    void disableVisualizationForAllRigidbodys() override;

private:
    void createWorld();
    RigidbodyId addRigidbody(std::unordered_map<uint32, Rigidbody>&, Rigidbody);
    Rigidbody* getRigidbody(const RigidbodyId&);
    const Rigidbody* getRigidbody(const RigidbodyId&) const;
    void clearRigidbody(const Rigidbody&);

private:
    std::unique_ptr<BulletDebugDrawer> bulletDebugDrawer_;
    std::unique_ptr<btDynamicsWorld> btDynamicWorld;
    std::unique_ptr<btBroadphaseInterface> btBroadPhase;
    std::unique_ptr<btConstraintSolver> btSolver;
    std::unique_ptr<btCollisionConfiguration> collisionConfiguration;
    std::unique_ptr<btDispatcher> btDispacher;
    std::unordered_map<uint32, Rigidbody> rigidBodies;
    std::unordered_map<uint32, Rigidbody> staticRigidBodies;
    std::unordered_map<uint32, std::unique_ptr<Shape>> shapes_;

    float simulationStep_;
    bool simualtePhysics_;
    Utils::IdPool idPool_;

    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
