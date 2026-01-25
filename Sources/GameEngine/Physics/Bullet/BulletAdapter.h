#pragma once
#include <Utils/IdPool.h>

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

#include <memory>

#include "GameEngine/Physics/IPhysicsApi.h"
#include "Task.h"

class btDynamicsWorld;
class btBroadphaseInterface;
class btConstraintSolver;
class btCollisionConfiguration;
class btDispatcher;

namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi

namespace GameEngine
{
namespace Physics
{
class BulletDebugDrawer;
namespace Bullet
{
struct Rigidbody;

class BulletAdapter : public IPhysicsApi
{
public:
    BulletAdapter();
    ~BulletAdapter() override;

    void Simulate(float) override;
    const GraphicsApi::LineMesh& DebugDraw() override;
    void EnableSimulation() override;
    void DisableSimulation() override;
    ShapeId CreateBoxColider(const PositionOffset&, const Scale&, const Size&) override;
    ShapeId CreateCylinderColider(const PositionOffset&, const Scale&, const Size&) override;
    ShapeId CreateSphereColider(const PositionOffset&, const Scale&, Radius) override;
    ShapeId CreateCapsuleColider(const PositionOffset&, const Scale&, Radius, float height) override;
    ShapeId CreateTerrainColider(const PositionOffset&, const Scale&, const HeightMap& heightMap) override;
    ShapeId CreateMeshCollider(const PositionOffset&, const std::vector<float>& data, const IndicesVector& indicies, const vec3&,
                               bool) override;
    RigidbodyId CreateRigidbody(const ShapeId&, GameObject&, const RigidbodyProperties&, float, bool&) override;
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
    void Translate(const RigidbodyId&, const vec3&) override;
    void SetRigidbodyScale(const RigidbodyId&, const vec3&) override;
    void SetShapeScale(const ShapeId&, const vec3&) override;
    std::optional<Quaternion> GetRotation(const RigidbodyId&) const override;
    std::optional<common::Transform> GetTransfrom(const RigidbodyId&) const override;
    std::optional<RayHit> RayTest(const vec3&, const vec3&) const override;
    void enableVisualizatedRigidbody(const RigidbodyId&) override;
    void disableVisualizatedRigidbody(const RigidbodyId&) override;
    void enableVisualizationForAllRigidbodys() override;
    void disableVisualizationForAllRigidbodys() override;
    CollisionSubId setCollisionCallback(const RigidbodyId&, const CollisionDetection&) override;
    void celarCollisionCallback(const CollisionSubId&) override;

private:
    void createWorld();
    void clearRigidbody(const Rigidbody&);
    void RemoveRigidBodyImpl(const RigidbodyId&);
    IdType addTask(Task::Action, std::optional<IdType> = std::nullopt);
    void removeTask(IdType);
    void executeTasks();
    CollisionContactInfos contactTest(const Rigidbody&);

private:
    std::unique_ptr<BulletDebugDrawer> bulletDebugDrawer_;
    std::unique_ptr<btDynamicsWorld> btDynamicWorld;
    std::unique_ptr<btBroadphaseInterface> btBroadPhase;
    std::unique_ptr<btConstraintSolver> btSolver;
    std::unique_ptr<btCollisionConfiguration> collisionConfiguration;
    std::unique_ptr<btDispatcher> btDispacher;

    float simulationStep_;
    bool simualtePhysics_;

    using Tasks = std::unordered_map<IdType, Task>;
    std::mutex dynamicWorldMutex;
    std::mutex tasksMutex;
    Tasks tasks;

    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;

    Utils::IdPool taskIdPool_;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
