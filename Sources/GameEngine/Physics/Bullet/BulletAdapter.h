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
    void SetSimulationStep(float step) override;
    void EnableSimulation() override;
    void DisableSimulation() override;
    uint32 CreateBoxColider(const vec3& positionOffset, const vec3& size) override;
    uint32 CreateSphereColider(const vec3& positionOffset, float radius) override;
    uint32 CreateCapsuleColider(const vec3& positionOffset, float radius, float height) override;
    // virtual void CreateMeshColider(vec3 positionOffset, std::vect) = 0;
    uint32 CreateTerrainColider(const vec3& positionOffset, const HeightMap& heightMap, const vec3& scale) override;
    uint32 CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data, const IndicesVector& indicies,
        const vec3&) override;
    uint32 CreateRigidbody(uint32 shapeId, GameObject&, float mass, bool isStatic) override;
    void RemoveRigidBody(uint32 id) override;
    void RemoveShape(uint32 id) override;
    void SetVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity) override;
    void ApplyImpulse(uint32 rigidBodyId, const vec3& impulse) override;
    void IncreaseVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity) override;
    std::optional<vec3> GetVelocity(uint32 rigidBodyId) override;
    void SetAngularFactor(uint32 rigidBodyId, float value) override;
    void SetAngularFactor(uint32 rigidBodyId, const vec3& value) override;
    std::optional<vec3> GetAngularFactor(uint32 rigidBodyId) override;
    void SetRotation(uint32 rigidBodyId, const vec3&) override;
    void SetRotation(uint32 rigidBodyId, const Quaternion&) override;
    void SetPosition(uint32 rigidBodyId, const vec3&) override;
    std::optional<Quaternion> GetRotation(uint32 rigidBodyId) const override;
    std::optional<common::Transform> GetTransfrom(uint32 rigidBodyId) const override;
    std::optional<RayHit> RayTest(const vec3&, const vec3&) const override;
    void setVisualizatedRigidbody(uint32) override;
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
