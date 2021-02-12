#include "BulletAdapter.h"

#include <Logger/Log.h>

#include <algorithm>
#include <unordered_map>

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "Converter.h"
#include "DebugDrawer.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "Utils.h"
#include "Utils/Variant.h"
#include "btBulletDynamicsCommon.h"

namespace GameEngine
{
namespace Physics
{
namespace
{
const float TRANSFROM_CHANGED_EPSILON = std::numeric_limits<float>::epsilon();

struct Shape
{
    Shape() = default;
    Shape(std::unique_ptr<btCollisionShape> btShape)
        : btShape_(std::move(btShape))
        , positionOffset_(0.f, 0.f, 0.f)
    {
    }
    Shape(std::unique_ptr<btCollisionShape> btShape, const btVector3& offset)
        : btShape_(std::move(btShape))
        , positionOffset_(offset)
    {
    }

    virtual ~Shape() = default;

    std::unique_ptr<btCollisionShape> btShape_{nullptr};
    btVector3 positionOffset_{0.f, 0.f, 0.f};
};

struct MeshShape : public Shape
{
    MeshShape() = default;
    MeshShape(std::unique_ptr<btTriangleMesh> btMesh)
    : btMesh_(std::move(btMesh))
    {}

    ~MeshShape()
    {
        btShape_.reset();
        btMesh_.reset();
    }
    std::unique_ptr<btTriangleMesh> btMesh_;
};

struct Rigidbody
{
    std::unique_ptr<btRigidBody> btRigidbody_;
    GameObject& gameObject;
    btVector3& positionOffset_;
    uint32 shapeId{0};
};
}  // namespace

struct BulletAdapter::Pimpl
{
    Pimpl()
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
    std::unordered_map<uint32, std::unique_ptr<Shape>> shapes_;
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
    //    shapes_.erase(rigidbody.shapeId);
    {
        std::lock_guard<std::mutex> lk(worldMutex_);
        btDynamicWorld->removeRigidBody(rigidbody.btRigidbody_.get());
    }
    rigidBodies.erase(id);
}
BulletAdapter::BulletAdapter()
    : simulationStep_(1.f / 60.f)
    , simualtePhysics_(true)
    , id_(1)
{
    impl_.reset(new Pimpl());
}
BulletAdapter::~BulletAdapter()
{
    DEBUG_LOG("destructor");
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

    for (auto& [id, rigidbody] : impl_->rigidBodies)
    {
        auto newPosition = Convert(rigidbody.btRigidbody_->getWorldTransform().getOrigin() + rigidbody.positionOffset_);

        Quaternion newRotation = Convert(rigidbody.btRigidbody_->getWorldTransform().getRotation());

        auto l1 = glm::length(rigidbody.gameObject.GetWorldTransform().GetPosition() - newPosition);
        auto l2 = glm::length(rigidbody.gameObject.GetWorldTransform().GetRotation().value_ - newRotation);

        if (l1 > TRANSFROM_CHANGED_EPSILON or l2 > TRANSFROM_CHANGED_EPSILON)
        {
            rigidbody.gameObject.SetWorldPositionRotation(newPosition, newRotation);
        }
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
    impl_->shapes_.insert(
        {id_, std::make_unique<Shape>(std::make_unique<btBoxShape>(Convert(size)), Convert(positionOffset))});
    return id_++;
}
uint32 BulletAdapter::CreateSphereColider(const vec3& positionOffset, float radius)
{
    impl_->shapes_.insert(
        {id_, std::make_unique<Shape>(std::make_unique<btSphereShape>(radius), Convert(positionOffset))});
    return id_++;
}

uint32 BulletAdapter::CreateCapsuleColider(const vec3& positionOffset, float radius, float height)
{
    impl_->shapes_.insert(
        {id_, std::make_unique<Shape>(std::make_unique<btCapsuleShape>(radius, height), Convert(positionOffset))});
    return id_++;
}

uint32 BulletAdapter::CreateTerrainColider(const vec3& positionOffset, const HeightMap& heightMap, const vec3& scale)
{
    impl_->shapes_.insert({id_, std::make_unique<Shape>()});
    auto& shape = *impl_->shapes_.at(id_);

    std::visit(visitor{
                   [&](const std::vector<uint8>& data) {
                       shape.btShape_.reset(new btHeightfieldTerrainShape(
                           heightMap.GetImage().width, heightMap.GetImage().height, &data[0], 1.f,
                           heightMap.GetMinimumHeight(), heightMap.GetMaximumHeight(), 1, PHY_UCHAR, false));
                   },
                   [&](const std::vector<float>& data) {
                       shape.btShape_.reset(new btHeightfieldTerrainShape(
                           heightMap.GetImage().width, heightMap.GetImage().height, &data[0], 1.f,
                           heightMap.GetMinimumHeight(), heightMap.GetMaximumHeight(), 1, PHY_FLOAT, false));
                   },
                   [](std::monostate) { ERROR_LOG("Height map data is not set!."); },
               },
               heightMap.GetImage().getImageData());

    float scaleX = scale.x / static_cast<float>(heightMap.GetImage().width - 1);
    float scaleY = scale.z / static_cast<float>(heightMap.GetImage().height - 1);

    shape.btShape_->setLocalScaling(btVector3(scaleX, scale.y, scaleY));

    auto offset           = heightMap.GetMaximumHeight() * scale.y - (heightMap.GetDeltaHeight() * scale.y / 2.f);
    shape.positionOffset_ = Convert(positionOffset + vec3(0, offset, 0));
    return id_++;
}
uint32 BulletAdapter::CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data,
                                         const IndicesVector& indicies, const vec3& scale)
{
    auto meshShape = std::make_unique<MeshShape>(std::make_unique<btTriangleMesh>(true, false));
    auto btMesh = meshShape->btMesh_.get();

    for (uint32 i = 0; i < indicies.size(); i += 3)
    {
        auto i1 = 3 * indicies[i];
        auto i2 = 3 * indicies[i + 1];
        auto i3 = 3 * indicies[i + 2];
        btVector3 v0(data[i1], data[i1 + 1], data[i1 + 2]);
        btVector3 v1(data[i2], data[i2 + 1], data[i2 + 2]);
        btVector3 v2(data[i3], data[i3 + 1], data[i3 + 2]);
        btMesh->addTriangle(v0, v1, v2);
    }


    meshShape->btShape_ = std::make_unique<btBvhTriangleMeshShape>(btMesh, true, true);
    meshShape->btShape_->setLocalScaling(Convert(scale));
    meshShape->positionOffset_ = Convert(positionOffset);

    impl_->shapes_.insert({id_, std::move(meshShape)});

    // delete trimesh;

    //  {
    // btConvexHullShape* convexHull = new btConvexHullShape(&convertedVerts[0].getX(), convertedVerts.size(),
    // sizeof(btVector3));
    // convexHull->optimizeConvexHull();
    // if (m_data->m_flags & CUF_INITIALIZE_SAT_FEATURES)
    //{
    //    convexHull->initializePolyhedralFeatures();
    //}
    // convexHull->setMargin(gUrdfDefaultCollisionMargin);
    // convexHull->recalcLocalAabb();
    // convexHull->setLocalScaling(collision->m_geometry.m_meshScale);
    // }

    return id_++;
}
uint32 BulletAdapter::CreateRigidbody(uint32 shapeId, GameObject& gameObject, float mass, bool isStatic)
{
    auto shapeIter = impl_->shapes_.find(shapeId);
    if (shapeIter == impl_->shapes_.end() or not shapeIter->second)
    {
        ERROR_LOG("Shape not found");
        return 0;
    }
    auto& shape               = *shapeIter->second;
    btCollisionShape* btShape = shape.btShape_.get();
    btAssert((!btShape || btShape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState* myMotionState{nullptr};
    int flags = btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;

    if (isStatic)
    {
        myMotionState =
            new btDefaultMotionState(Convert(gameObject.GetWorldTransform(), Convert(shape.positionOffset_)));
        flags |= btCollisionObject::CF_STATIC_OBJECT;
    }
    else
    {
        btShape->calculateLocalInertia(mass, localInertia);
        myMotionState = new btDefaultMotionState(Convert(gameObject.GetWorldTransform()));
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape.btShape_.get(), localInertia);
    Rigidbody body{std::make_unique<btRigidBody>(cInfo), gameObject, shape.positionOffset_, shapeId};
    body.btRigidbody_->setCollisionFlags(body.btRigidbody_->getCollisionFlags() | flags);
    impl_->AddRigidbody(isStatic ? impl_->staticRigidBodies : impl_->rigidBodies, id_, std::move(body));
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

void BulletAdapter::ApplyImpulse(uint32 rigidBodyId, const vec3& impulse)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return;
    }

    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->applyCentralImpulse(Convert(impulse));
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
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        ERROR_LOG("Rigidbody not found !" + std::to_string(rigidBodyId));
        return;
    }

    if (compare(value, 0.f))
    {
        DEBUG_LOG("DISABLE_DEACTIVATION btRigidbody_ : " + std::to_string(rigidBodyId));
        impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setActivationState(DISABLE_DEACTIVATION);
    }

    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setAngularFactor(value);
}

void BulletAdapter::SetAngularFactor(uint32 rigidBodyId, const vec3& value)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        ERROR_LOG("Rigidbody not found !" + std::to_string(rigidBodyId));
        return;
    }

    DEBUG_LOG("DISABLE_DEACTIVATION btRigidbody_ : " + std::to_string(rigidBodyId));
    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setActivationState(DISABLE_DEACTIVATION);
    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setAngularFactor(Convert(value));
}

std::optional<vec3> BulletAdapter::GetAngularFactor(uint32 rigidBodyId)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        ERROR_LOG("Rigidbody not found !" + std::to_string(rigidBodyId));
        return std::nullopt;
    }

    return Convert(impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getAngularFactor());
}
void BulletAdapter::RemoveRigidBody(uint32 id)
{
    impl_->RemoveRigidBody(impl_->rigidBodies, id);
    impl_->RemoveRigidBody(impl_->staticRigidBodies, id);
}
void BulletAdapter::RemoveShape(uint32 id)
{
    auto shapeIter = impl_->shapes_.find(id);
    if (shapeIter != impl_->shapes_.end())
    {
        impl_->shapes_.erase(id);
    }
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

std::optional<Quaternion> BulletAdapter::GetRotation(uint32 rigidBodyId) const
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return std::nullopt;
    }
    return Convert(impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getWorldTransform().getRotation());
}

std::optional<common::Transform> BulletAdapter::GetTransfrom(uint32 rigidBodyId) const
{
    auto rigidbodyIter = impl_->rigidBodies.find(rigidBodyId);

    if (rigidbodyIter == impl_->rigidBodies.end())
    {
        return std::nullopt;
    }
    return Convert(rigidbodyIter->second.btRigidbody_->getWorldTransform());
}
std::optional<RayHit> BulletAdapter::RayTest(const vec3& from, const vec3& to) const
{
    btVector3 btFrom = Convert(from);
    btVector3 btTo   = Convert(to);
    btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

    impl_->btDynamicWorld->rayTest(btFrom, btTo, res);

    if (res.hasHit())
    {
        return RayHit{Convert(res.m_hitPointWorld), Convert(res.m_hitNormalWorld)};
    }

    return std::nullopt;
}
void BulletAdapter::setVisualizatedRigidbody(uint32 rigidBodyId)
{
    auto rigidbodyIter = impl_->rigidBodies.find(rigidBodyId);

    if (rigidbodyIter != impl_->rigidBodies.end())
    {
        disableVisualizationForAllRigidbodys();
        auto& body = *rigidbodyIter->second.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
    }
}
void BulletAdapter::enableVisualizationForAllRigidbodys()
{
    for (auto& rigidbody : impl_->rigidBodies)
    {
        auto& body = *rigidbody.second.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
    }
}
void BulletAdapter::disableVisualizationForAllRigidbodys()
{
    for (auto& rigidbody : impl_->rigidBodies)
    {
        auto& body = *rigidbody.second.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }
}
}  // namespace Physics

}  // namespace GameEngine
