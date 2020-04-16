#pragma once
#include <memory>
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Physics
{
class BachPhysicsAdapter : public IPhysicsApi
{
public:
    BachPhysicsAdapter();
    ~BachPhysicsAdapter();
    void Simulate() override;
    const GraphicsApi::LineMesh& DebugDraw() override;
    void SetSimulationStep(float step) override;
    void EnableSimulation() override;
    void DisableSimulation() override;
    uint32 CreateBoxColider(const vec3& positionOffset, const vec3& size) override;
    uint32 CreateSphereColider(const vec3& positionOffset, float radius) override;
    uint32 CreateCapsuleColider(const vec3& positionOffset, float radius, float height) override;
    // void CreateMeshColider(vec3 positionOffset, std::vect) = 0;
    uint32 CreateTerrainColider(const vec3& positionOffset, const vec2ui& size, const std::vector<float>& data,
                                const vec3& scale) override;
    uint32 CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data, const IndicesVector& indicies,
                              float scaleFactor) override;
    uint32 CreateRigidbody(uint32 shapeId, common::Transform&, float mass, bool isStatic) override;
    void SetVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity) override;
    void ApplyImpulse(uint32 rigidBodyId, const vec3& impulse) override;
    void IncreaseVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity) override;
    std::optional<vec3> GetVelocity(uint32 rigidBodyId) override;
    void SetAngularFactor(uint32 rigidBodyId, float value) override;
    void SetAngularFactor(uint32 rigidBodyId, const vec3& value) override;
    std::optional<vec3> GetAngularFactor(uint32 rigidBodyId) override;
    void RemoveRigidBody(uint32 id) override;
    void SetRotation(uint32 rigidBodyId, const vec3&) override;
    void SetRotation(uint32 rigidBodyId, const Quaternion&) override;
    void SetPosition(uint32 rigidBodyId, const vec3&) override;
    std::optional<Quaternion> GetRotation(uint32 rigidBodyId) const override;
    std::optional<common::Transform> GetTransfrom(uint32 rigidBodyId) const override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
    GraphicsApi::LineMesh lineMesh_;
};
}  // namespace Physics
}  // namespace GameEngine
