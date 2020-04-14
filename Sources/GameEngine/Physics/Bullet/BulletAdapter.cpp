#include "BulletAdapter.h"
#include <Logger/Log.h>
#include <algorithm>
#include <unordered_map>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "Converter.h"
#include "DebugDrawer.h"
#include "Utils.h"
#include "btBulletDynamicsCommon.h"

namespace GameEngine
{
namespace Physics
{
struct Shape
{
    std::unique_ptr<btCollisionShape> shape_;
    btVector3 positionOffset_;
};

struct Rigidbody
{
    std::unique_ptr<btRigidBody> btRigidbody_;
    btVector3* positionOffset_;
    uint32 shapeId = 0;
    bool isStatic  = false;
};

struct BulletAdapter::Pimpl
{
    Pimpl(GraphicsApi::IGraphicsApi& graphicsApi)
        : graphicsApi_(graphicsApi)
    {
        collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
        btDispacher            = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
        btBroadPhase           = std::make_unique<btDbvtBroadphase>();
        btSolver               = std::make_unique<btSequentialImpulseConstraintSolver>();
        btDynamicWorld         = std::make_unique<btDiscreteDynamicsWorld>(btDispacher.get(), btBroadPhase.get(),
                                                                   btSolver.get(), collisionConfiguration.get());
        btDynamicWorld->setGravity(btVector3(0, -10, 0));

        bulletDebugDrawer_ = std::make_unique<BulletDebugDrawer>();
        bulletDebugDrawer_->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
        btDynamicWorld->setDebugDrawer(bulletDebugDrawer_.get());
    }

    void AddRigidbody(std::unordered_map<uint32, Rigidbody>& target, uint32 id, Rigidbody newBody);
    void RemoveRigidBody(std::unordered_map<uint32, Rigidbody>& target, uint32 id);

    std::unique_ptr<BulletDebugDrawer> bulletDebugDrawer_;
    std::unique_ptr<btDynamicsWorld> btDynamicWorld;
    std::unique_ptr<btBroadphaseInterface> btBroadPhase;
    std::unique_ptr<btConstraintSolver> btSolver;
    std::unique_ptr<btCollisionConfiguration> collisionConfiguration;
    std::unique_ptr<btDispatcher> btDispacher;
    std::unordered_map<uint32, Rigidbody> rigidBodies;
    std::unordered_map<uint32, Rigidbody> staticRigidBodies;
    std::unordered_map<uint32, common::Transform*> transforms;
    std::unordered_map<uint32, Shape> shapes_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::mutex worldMutex_;
};
void BulletAdapter::Pimpl::AddRigidbody(std::unordered_map<uint32, Rigidbody>& target, uint32 id, Rigidbody newBody)
{
    target.insert({id, std::move(newBody)});
    auto& body = target.at(id).btRigidbody_;
    body->setUserIndex(-1);

    std::lock_guard<std::mutex> lk(worldMutex_);
    btDynamicWorld->addRigidBody(body.get());
    btDynamicWorld->updateSingleAabb(body.get());
}
void BulletAdapter::Pimpl::RemoveRigidBody(std::unordered_map<uint32, Rigidbody>& target, uint32 id)
{
    if (!target.count(id))
    {
        return;
    }

    auto& rigidbody = target.at(id);
    shapes_.erase(rigidbody.shapeId);
    {
        std::lock_guard<std::mutex> lk(worldMutex_);
        btDynamicWorld->removeRigidBody(rigidbody.btRigidbody_.get());
    }
    rigidBodies.erase(id);
    transforms.erase(id);
}
BulletAdapter::BulletAdapter(GraphicsApi::IGraphicsApi& graphicsApi)
    : simulationStep_(1.f / 60.f)
    , simualtePhysics_(true)
    , id_(1)
{
    impl_.reset(new Pimpl(graphicsApi));
}
BulletAdapter::~BulletAdapter()
{
}
void BulletAdapter::SetSimulationStep(float step)
{
    simulationStep_ = step;
}
void BulletAdapter::EnableSimulation()
{
    simualtePhysics_ = true;
}
void BulletAdapter::Simulate()
{
    if (!simualtePhysics_)
    {
        return;
    }

    impl_->btDynamicWorld->stepSimulation(simulationStep_);

    for (auto& pair : impl_->rigidBodies)
    {
        auto& transform = impl_->transforms.at(pair.first);
        auto& rigidbody = pair.second;

        auto newPosition = rigidbody.btRigidbody_->getWorldTransform().getOrigin() + *rigidbody.positionOffset_;

        Quaternion newRotation = Convert(rigidbody.btRigidbody_->getWorldTransform().getRotation());
        transform->SetPositionAndRotation(Convert(newPosition),
                                          newRotation * glm::angleAxis(glm::radians(180.f), vec3(0.f, 0.f, 1.f)));
    }
}
const GraphicsApi::LineMesh& BulletAdapter::DebugDraw()
{
    impl_->bulletDebugDrawer_->clear();
    {
        std::lock_guard<std::mutex> lk(impl_->worldMutex_);
        impl_->btDynamicWorld->debugDrawWorld();
    }
    return impl_->bulletDebugDrawer_->getMesh();
}
void BulletAdapter::DisableSimulation()
{
    simualtePhysics_ = false;
}
uint32 BulletAdapter::CreateBoxColider(const vec3& positionOffset, const vec3& size)
{
    impl_->shapes_.insert({id_, Shape()});
    impl_->shapes_.at(id_).shape_.reset(new btBoxShape(Convert(size)));
    impl_->shapes_.at(id_).positionOffset_ = Convert(positionOffset);
    return id_++;
}
uint32 BulletAdapter::CreateSphereColider(const vec3& positionOffset, float radius)
{
    impl_->shapes_.insert({id_, Shape()});
    impl_->shapes_.at(id_).shape_.reset(new btSphereShape(radius));
    impl_->shapes_.at(id_).positionOffset_ = Convert(positionOffset);
    return id_++;
}

uint32 BulletAdapter::CreateCapsuleColider(const vec3& positionOffset, float radius, float height)
{
    impl_->shapes_.insert({id_, Shape()});
    impl_->shapes_.at(id_).shape_.reset(new btCapsuleShape(radius, height));
    impl_->shapes_.at(id_).positionOffset_ = Convert(positionOffset);
    return id_++;
}

uint32 BulletAdapter::CreateTerrainColider(const vec3& positionOffset, const vec2ui& size,
                                           const std::vector<float>& data, const vec3& scale)
{
    auto maxElementIter = std::max_element(data.begin(), data.end());
    auto maxElement     = maxElementIter != data.end() ? *maxElementIter : 0.f;

    impl_->shapes_.insert({id_, Shape()});
    impl_->shapes_.at(id_).shape_.reset(
        new btHeightfieldTerrainShape(size.x, size.y, &data[0], 1.f, 0.f, maxElement, 1, PHY_FLOAT, false));

    float scaleX = scale.x / static_cast<float>(size.x);
    float scaleY = scale.z / static_cast<float>(size.y);

    impl_->shapes_.at(id_).shape_->setLocalScaling(btVector3(scaleX, scale.y, scaleY));
    impl_->shapes_.at(id_).positionOffset_ = Convert(positionOffset);

    return id_++;
}
uint32 BulletAdapter::CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data,
                                         const IndicesVector& indicies, float scaleFactor)
{
    impl_->shapes_.insert({id_, Shape()});
    auto& shape = impl_->shapes_.at(id_);

    auto trimesh = new btTriangleMesh();

    for (uint32 i = 0; i < indicies.size(); i += 9)
    {
        btVector3 v0(data[i], data[i + 1], data[i + 2]);
        btVector3 v1(data[i + 3], data[i + 4], data[i + 5]);
        btVector3 v2(data[i + 6], data[i + 7], data[i + 8]);

        trimesh->addTriangle(v0, v1, v2);
    }
    btConvexShape* tmpshape = new btConvexTriangleMeshShape(trimesh);
    tmpshape->setLocalScaling(btVector3(scaleFactor, scaleFactor, scaleFactor));

    shape.shape_          = std::unique_ptr<btCollisionShape>(tmpshape);
    shape.positionOffset_ = Convert(positionOffset);

    btShapeHull* hull = new btShapeHull(tmpshape);
    btScalar margin   = tmpshape->getMargin();
    hull->buildHull(margin);
    tmpshape->setUserPointer(hull);

    return id_++;
}
uint32 BulletAdapter::CreateRigidbody(uint32 shapeId, common::Transform& transform, float mass, bool isStatic)
{
    if (impl_->shapes_.count(shapeId) == 0)
    {
        return 0;
    }

    btCollisionShape* shape = impl_->shapes_.at(shapeId).shape_.get();
    // shape->setLocalScaling(Convert(transform.GetScale()));
    btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    btVector3 localInertia(0, 0, 0);

    if (!isStatic)
        shape->calculateLocalInertia(mass, localInertia);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(Convert(transform));

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

    Rigidbody body{std::make_unique<btRigidBody>(cInfo), &impl_->shapes_.at(shapeId).positionOffset_, shapeId, true};

    impl_->AddRigidbody(impl_->rigidBodies, id_, std::move(body));
    impl_->transforms[id_] = &transform;

    if (not isStatic)
    {
    }

    return id_++;
}
void BulletAdapter::SetVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity)
{
    if (!impl_->rigidBodies.count(rigidBodyId))
    {
        return;
    }

    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setLinearVelocity(Convert(velocity));
}
void BulletAdapter::IncreaseVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return;
    }

    const auto& v = impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getLinearVelocity();
    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setLinearVelocity(v + Convert(velocity));
}
std::optional<vec3> BulletAdapter::GetVelocity(uint32 rigidBodyId)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return {};
    }

    return Convert(impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getLinearVelocity());
}
void BulletAdapter::SetAngularFactor(uint32 rigidBodyId, float value)
{
    if (!impl_->rigidBodies.count(rigidBodyId))
    {
        return;
    }

    if (compare(value, 0.f))
    {
        DEBUG_LOG("DISABLE_DEACTIVATION btRigidbody_");
        impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setActivationState(DISABLE_DEACTIVATION);
    }

    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setAngularFactor(value);
}
void BulletAdapter::RemoveRigidBody(uint32 id)
{
    impl_->RemoveRigidBody(impl_->rigidBodies, id);
    impl_->RemoveRigidBody(impl_->staticRigidBodies, id);
}
void BulletAdapter::SetRotation(uint32 rigidBodyId, const vec3& rotation)
{
    btQuaternion qt;
    qt.setEuler(rotation.y, rotation.x, rotation.z);
    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getWorldTransform().setRotation(qt);
}
void BulletAdapter::SetRotation(uint32 rigidBodyId, const Quaternion& rotation)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return;
    }

    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getWorldTransform().setRotation(Convert(rotation));
}
void BulletAdapter::SetPosition(uint32 rigidBodyId, const vec3& position)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return;
    }
    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getWorldTransform().setOrigin(Convert(position));
}
}  // namespace Physics

}  // namespace GameEngine
