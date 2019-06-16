#pragma once
#include <memory>
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GraphicsApi
{
    class IGraphicsApi;
} // GraphicsApi

namespace GameEngine
{

namespace Physics
{
class BulletAdapter : public IPhysicsApi
{
public:
    BulletAdapter(GraphicsApi::IGraphicsApi& graphicsApi);
    ~BulletAdapter();
    virtual void Simulate() override;
    virtual void DebugDraw(const mat4& viewMatrix, const mat4& projectionMatrix) override;
    virtual void SetSimulationStep(float step) override;
    virtual void EnableSimulation() override;
    virtual void DisableSimulation() override;
    virtual uint32 CreateBoxColider(const vec3& positionOffset, const vec3& size) override;
    virtual uint32 CreateSphereColider(const vec3& positionOffset, float radius) override;
    // virtual void CreateMeshColider(vec3 positionOffset, std::vect) = 0;
    virtual uint32 CreateTerrainColider(const vec3& positionOffset, const vec2ui& size, const std::vector<float>& data,
                                        const vec3& scale) override;
    virtual uint32 CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data,
                                      const IndicesVector& indicies, float scaleFactor) override;
    virtual uint32 CreateRigidbody(uint32 shapeId, common::Transform&, float mass, bool isStatic) override;
    virtual void SetVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity) override;
    virtual void RemoveRigidBody(uint32 id) override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
    float simulationStep_;
    bool simualtePhysics_;
    uint32 id_;
};
}  // namespace Physics
}  // namespace GameEngine
