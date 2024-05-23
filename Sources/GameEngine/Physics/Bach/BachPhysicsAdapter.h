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
    void Simulate(float) override;
    const GraphicsApi::LineMesh& DebugDraw() override;
    void EnableSimulation() override;
    void DisableSimulation() override;
    ShapeId CreateBoxColider(const vec3& positionOffset, const Scale&, const Size&) override;
    ShapeId CreateSphereColider(const vec3& positionOffset, const Scale&, Radius) override;
    ShapeId CreateCapsuleColider(const vec3& positionOffset, const Scale&, Radius, float) override;
    // void CreateMeshColider(vec3 positionOffset, std::vect) = 0;
    ShapeId CreateTerrainColider(const vec3& positionOffset, const Scale&, const HeightMap&) override;
    ShapeId CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data,
                               const IndicesVector& indicies, const vec3&, bool) override;
    RigidbodyId CreateRigidbody(const ShapeId&, GameObject&, const RigidbodyProperties&, float mass, bool&) override;
    void RemoveRigidBody(const RigidbodyId&) override;
    void RemoveShape(const ShapeId&) override;
    void SetVelocityRigidbody(const RigidbodyId&, const vec3& velocity) override;
    void ApplyImpulse(const RigidbodyId&, const vec3& impulse) override;
    void IncreaseVelocityRigidbody(const RigidbodyId&, const vec3& velocity) override;
    std::optional<vec3> GetVelocity(const RigidbodyId&) override;
    void SetAngularFactor(const RigidbodyId&, float value) override;
    void SetAngularFactor(const RigidbodyId&, const vec3& value) override;
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
    Physics::CollisionSubId setCollisionCallback(const RigidbodyId&,
                                                 std::function<void(const CollisionContactInfo&)>) override;
    void celarCollisionCallback(const CollisionSubId&) override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
    GraphicsApi::LineMesh lineMesh_;
};
}  // namespace Physics
}  // namespace GameEngine
