#pragma once
#include <memory>

#include "GameEngine/Physics/IPhysicsApi.h"

namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi

namespace GameEngine
{
namespace Physics
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
    uint32 CreateBoxColider(const PositionOffset&, const vec3& scale, const vec3& size) override;
    uint32 CreateSphereColider(const PositionOffset&, const vec3& scale, float radius) override;
    uint32 CreateCapsuleColider(const PositionOffset&, const vec3& scale, float radius, float height) override;
    uint32 CreateTerrainColider(const PositionOffset&, const vec3& scale, const HeightMap& heightMap) override;
    uint32 CreateMeshCollider(const PositionOffset&, const std::vector<float>& data, const IndicesVector& indicies,
                              const vec3&, bool) override;
    uint32 CreateRigidbody(ShapeId, GameObject&, float, bool, bool&) override;
    void RemoveRigidBody(RigidbodyId) override;
    void RemoveShape(ShapeId) override;
    void SetVelocityRigidbody(RigidbodyId, const vec3& velocity) override;
    void ApplyImpulse(RigidbodyId, const vec3& impulse) override;
    void IncreaseVelocityRigidbody(RigidbodyId, const vec3&) override;
    std::optional<vec3> GetVelocity(RigidbodyId) override;
    void SetAngularFactor(RigidbodyId, float) override;
    void SetAngularFactor(RigidbodyId, const vec3&) override;
    std::optional<vec3> GetAngularFactor(RigidbodyId) override;
    void SetRotation(RigidbodyId, const vec3&) override;
    void SetRotation(RigidbodyId, const Quaternion&) override;
    void SetPosition(RigidbodyId, const vec3&) override;
    void SetRigidbodyScale(RigidbodyId, const vec3&) override;
    void SetShapeScale(ShapeId, const vec3&) override;
    std::optional<Quaternion> GetRotation(RigidbodyId) const override;
    std::optional<common::Transform> GetTransfrom(RigidbodyId) const override;
    std::optional<RayHit> RayTest(const vec3&, const vec3&) const override;
    void setVisualizatedRigidbody(RigidbodyId) override;
    void enableVisualizationForAllRigidbodys() override;
    void disableVisualizationForAllRigidbodys() override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
    float simulationStep_;
    bool simualtePhysics_;
    uint32 id_;
};
}  // namespace Physics
}  // namespace GameEngine
