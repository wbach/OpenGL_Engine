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
    virtual void Simulate() override;
    virtual void DebugDraw(const mat4& viewMatrix, const mat4& projectionMatrix) override;
    virtual void SetSimulationStep(float step) override;
    virtual void EnableSimulation() override;
    virtual void DisableSimulation() override;
    virtual uint32 CreateBoxColider(const vec3& positionOffset, const vec3& size) override;
    virtual uint32 CreateSphereColider(const vec3& positionOffset, float radius) override;
    virtual uint32 CreateCapsuleColider(const vec3& positionOffset, float radius, float height) override;
    // virtual void CreateMeshColider(vec3 positionOffset, std::vect) = 0;
    virtual uint32 CreateTerrainColider(const vec3& positionOffset, const vec2ui& size, const std::vector<float>& data,
        const vec3& scale) override;
    virtual uint32 CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data,
                                      const IndicesVector& indicies, float scaleFactor) override;
    virtual uint32 CreateRigidbody(uint32 shapeId, common::Transform&, float mass, bool isStatic) override;
    virtual void SetVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity) override;
    virtual void IncreaseVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity) override;
    virtual std::optional<vec3> GetVelocity(uint32 rigidBodyId) override;
    virtual void SetAngularFactor(uint32 rigidBodyId, float value) override;
    virtual void RemoveRigidBody(uint32 id) override;
    virtual void SetRotation(uint32 rigidBodyId, const vec3&) override;
    virtual void SetRotation(uint32 rigidBodyId, const Quaternion&) override;
    virtual void SetPosition(uint32 rigidBodyId, const vec3&) override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
};
}  // namespace Physics
}  // namespace GameEngine
