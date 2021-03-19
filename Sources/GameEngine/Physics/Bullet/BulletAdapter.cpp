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
        , dynamicShapeAllowed_{true}
    {
    }
    Shape(std::unique_ptr<btCollisionShape> btShape, const btVector3& offset)
        : btShape_(std::move(btShape))
        , positionOffset_(offset)
        , dynamicShapeAllowed_{true}
    {
    }

    virtual ~Shape() = default;

    std::unique_ptr<btCollisionShape> btShape_{nullptr};
    btVector3 positionOffset_{0.f, 0.f, 0.f};
    bool dynamicShapeAllowed_{true};
};

struct MeshShape : public Shape
{
    MeshShape() = default;
    MeshShape(std::unique_ptr<btTriangleMesh> btMesh)
        : btMesh_(std::move(btMesh))
    {
        dynamicShapeAllowed_ = false;
    }

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
    bool& isUpdating_;
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

    ~Pimpl()
    {
        for (const auto& [id, body] : rigidBodies)
            btDynamicWorld->removeRigidBody(body.btRigidbody_.get());
        for (const auto& [id, body] : staticRigidBodies)
            btDynamicWorld->removeRigidBody(body.btRigidbody_.get());
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


    btDynamicWorld->addRigidBody(body.get());
    btDynamicWorld->updateSingleAabb(body.get());
}
void BulletAdapter::Pimpl::RemoveRigidBody(std::unordered_map<uint32, Rigidbody>& target, uint32 id)
{
    std::lock_guard<std::mutex> lk(worldMutex_);
    auto iter = target.find(id);
    if (iter == target.end())
    {
        return;
    }

    //    shapes_.erase(rigidbody.shapeId);
    btDynamicWorld->removeRigidBody(iter->second.btRigidbody_.get());
    target.erase(iter);
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
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (simualtePhysics_)
    {
        impl_->btDynamicWorld->stepSimulation(simulationStep_);

        for (auto& [id, rigidbody] : impl_->rigidBodies)
        {
            auto newPosition =
                Convert(rigidbody.btRigidbody_->getWorldTransform().getOrigin() - rigidbody.positionOffset_);

            Quaternion newRotation = Convert(rigidbody.btRigidbody_->getWorldTransform().getRotation());

            auto l1 = glm::length(rigidbody.gameObject.GetWorldTransform().GetPosition() - newPosition);
            auto l2 = glm::length(rigidbody.gameObject.GetWorldTransform().GetRotation().value_ - newRotation);

            if (l1 > TRANSFROM_CHANGED_EPSILON or l2 > TRANSFROM_CHANGED_EPSILON)
            {
                rigidbody.isUpdating_ = true;
                rigidbody.gameObject.SetWorldPositionRotation(newPosition, newRotation);
                rigidbody.isUpdating_ = false;
            }
        }
    }
}
const GraphicsApi::LineMesh& BulletAdapter::DebugDraw()
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    impl_->bulletDebugDrawer_->clear();
    {
        impl_->btDynamicWorld->debugDrawWorld();
    }
    return impl_->bulletDebugDrawer_->getMesh();
}
void BulletAdapter::DisableSimulation()
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    simualtePhysics_ = false;
}
uint32 BulletAdapter::CreateBoxColider(const PositionOffset& positionOffset, const vec3& scale, const vec3& size)
{
    DEBUG_LOG(std::to_string(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)));
    impl_->shapes_.insert({id_, std::make_unique<Shape>(std::make_unique<btBoxShape>(Convert(size)),
                                                        Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)))});
    return id_++;
}
uint32 BulletAdapter::CreateSphereColider(const PositionOffset& positionOffset, const vec3& scale, float radius)
{
    // DEBUG_LOG(std::to_string(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)));
    impl_->shapes_.insert({id_, std::make_unique<Shape>(std::make_unique<btSphereShape>(radius),
                                                        Convert(positionOffset /*+ vec3(0.f, scale.y / 2.f, 0.f)*/))});
    return id_++;
}

uint32 BulletAdapter::CreateCapsuleColider(const PositionOffset& positionOffset, const vec3& scale, float radius,
                                           float height)
{
    impl_->shapes_.insert({id_, std::make_unique<Shape>(std::make_unique<btCapsuleShape>(radius, height / 2.f),
                                                        Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)))});

    return id_++;
}

uint32 BulletAdapter::CreateTerrainColider(const PositionOffset& positionOffset, const vec3& scale,
                                           const HeightMap& heightMap)
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
uint32 BulletAdapter::CreateMeshCollider(const PositionOffset& positionOffset, const std::vector<float>& data,
                                         const IndicesVector& indicies, const vec3& scale, bool autoOptimize)
{
    if (not autoOptimize)
    {
        auto meshShape = std::make_unique<MeshShape>(std::make_unique<btTriangleMesh>(true, false));
        auto btMesh    = meshShape->btMesh_.get();

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
    }
    else
    {
        auto hullShape = std::make_unique<btConvexHullShape>();

        for (uint32 i = 0; i < indicies.size(); i += 3)
        {
            auto i1 = i;
            btVector3 v0(data[i1], data[i1 + 1], data[i1 + 2]);
            hullShape->addPoint(v0, false);
        }

        hullShape->optimizeConvexHull();
        hullShape->recalcLocalAabb();
        hullShape->setLocalScaling(Convert(scale));

        impl_->shapes_.insert({id_, std::move(std::make_unique<Shape>(std::move(hullShape), Convert(positionOffset)))});
    }

    return id_++;
}
uint32 BulletAdapter::CreateRigidbody(ShapeId shapeId, GameObject& gameObject, float mass, bool isStatic,
                                      bool& isUpdating)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);

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

    if (isStatic or not shape.dynamicShapeAllowed_)
    {
        flags |= btCollisionObject::CF_STATIC_OBJECT;
    }
    else
    {
        btShape->calculateLocalInertia(mass, localInertia);
    }

    myMotionState = new btDefaultMotionState(Convert(gameObject.GetWorldTransform(), Convert(shape.positionOffset_)));
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape.btShape_.get(), localInertia);
    Rigidbody body{std::make_unique<btRigidBody>(cInfo), gameObject, shape.positionOffset_, shapeId, isUpdating};
    body.btRigidbody_->setCollisionFlags(body.btRigidbody_->getCollisionFlags() | flags);
    body.btRigidbody_->setFriction(1);
    impl_->AddRigidbody(isStatic ? impl_->staticRigidBodies : impl_->rigidBodies, id_, std::move(body));
    return id_++;
}
void BulletAdapter::SetVelocityRigidbody(RigidbodyId rigidBodyId, const vec3& velocity)
{
    if (!impl_->rigidBodies.count(rigidBodyId))
    {
        return;
    }

    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setLinearVelocity(Convert(velocity));
}

void BulletAdapter::ApplyImpulse(RigidbodyId rigidBodyId, const vec3& impulse)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return;
    }

    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->applyCentralImpulse(Convert(impulse));
}
void BulletAdapter::IncreaseVelocityRigidbody(RigidbodyId rigidBodyId, const vec3& velocity)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return;
    }

    const auto& v = impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getLinearVelocity();
    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->setLinearVelocity(v + Convert(velocity));
}
std::optional<vec3> BulletAdapter::GetVelocity(RigidbodyId rigidBodyId)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return {};
    }

    return Convert(impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getLinearVelocity());
}
void BulletAdapter::SetAngularFactor(RigidbodyId rigidBodyId, float value)
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

void BulletAdapter::SetAngularFactor(RigidbodyId rigidBodyId, const vec3& value)
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

std::optional<vec3> BulletAdapter::GetAngularFactor(RigidbodyId rigidBodyId)
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        ERROR_LOG("Rigidbody not found !" + std::to_string(rigidBodyId));
        return std::nullopt;
    }

    return Convert(impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getAngularFactor());
}
void BulletAdapter::RemoveRigidBody(RigidbodyId id)
{
    impl_->RemoveRigidBody(impl_->rigidBodies, id);
    impl_->RemoveRigidBody(impl_->staticRigidBodies, id);
}
void BulletAdapter::RemoveShape(ShapeId id)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    {
        auto iter = std::find_if(impl_->rigidBodies.begin(), impl_->rigidBodies.end(),
            [id](auto& pair) { return pair.second.shapeId == id; });

        if (iter != impl_->rigidBodies.end())
        {
            impl_->btDynamicWorld->removeRigidBody(iter->second.btRigidbody_.get());
            impl_->rigidBodies.erase(iter);
        }
    }
    {
        auto iter = std::find_if(impl_->staticRigidBodies.begin(), impl_->staticRigidBodies.end(),
            [id](auto& pair) { return pair.second.shapeId == id; });

        if (iter != impl_->staticRigidBodies.end())
        {
            impl_->btDynamicWorld->removeRigidBody(iter->second.btRigidbody_.get());
            impl_->staticRigidBodies.erase(iter);
        }
    }

    auto shapeIter = impl_->shapes_.find(id);
    if (shapeIter != impl_->shapes_.end())
    {
        impl_->shapes_.erase(id);
    }
}
void BulletAdapter::SetRotation(RigidbodyId rigidBodyId, const vec3& rotation)
{
    btQuaternion qt;
    qt.setEuler(rotation.y, rotation.x, rotation.z);
    impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getWorldTransform().setRotation(qt);
}
void BulletAdapter::SetRotation(RigidbodyId rigidBodyId, const Quaternion& rotation)
{
    auto rIter = impl_->rigidBodies.find(rigidBodyId);
    if (rIter != impl_->rigidBodies.end())
    {
        rIter->second.btRigidbody_->getWorldTransform().setRotation(Convert(rotation));
        return;
    }

    auto sIter = impl_->staticRigidBodies.find(rigidBodyId);
    if (sIter != impl_->staticRigidBodies.end())
    {
        sIter->second.btRigidbody_->getWorldTransform().setRotation(Convert(rotation));
        return;
    }
}
void BulletAdapter::SetPosition(RigidbodyId rigidBodyId, const vec3& position)
{
    auto rIter = impl_->rigidBodies.find(rigidBodyId);
    if (rIter != impl_->rigidBodies.end())
    {
        rIter->second.btRigidbody_->getWorldTransform().setOrigin(Convert(position));
        return;
    }

    auto sIter = impl_->staticRigidBodies.find(rigidBodyId);
    if (sIter != impl_->staticRigidBodies.end())
    {
        sIter->second.btRigidbody_->getWorldTransform().setOrigin(Convert(position));
        return;
    }
}

void BulletAdapter::SetRigidbodyScale(RigidbodyId rigidBodyId, const vec3& position)
{
    {
        auto rIter = impl_->rigidBodies.find(rigidBodyId);
        if (rIter == impl_->rigidBodies.end())
        {
            return;
        }

        auto sIter = impl_->shapes_.find(rIter->second.shapeId);
        if (sIter != impl_->shapes_.end())
        {
            sIter->second->btShape_->setLocalScaling(Convert(position));
        }
    }

    {
        auto rIter = impl_->staticRigidBodies.find(rigidBodyId);
        if (rIter == impl_->staticRigidBodies.end())
        {
            return;
        }

        auto sIter = impl_->shapes_.find(rIter->second.shapeId);
        if (sIter != impl_->shapes_.end())
        {
            sIter->second->btShape_->setLocalScaling(Convert(position));
        }
    }
}

void BulletAdapter::SetShapeScale(ShapeId shapeId, const vec3& position)
{
    auto sIter = impl_->shapes_.find(shapeId);
    if (sIter != impl_->shapes_.end())
    {
        sIter->second->btShape_->setLocalScaling(Convert(position));
    }
}

std::optional<Quaternion> BulletAdapter::GetRotation(RigidbodyId rigidBodyId) const
{
    if (not impl_->rigidBodies.count(rigidBodyId))
    {
        return std::nullopt;
    }
    return Convert(impl_->rigidBodies.at(rigidBodyId).btRigidbody_->getWorldTransform().getRotation());
}

std::optional<common::Transform> BulletAdapter::GetTransfrom(RigidbodyId rigidBodyId) const
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
void BulletAdapter::setVisualizatedRigidbody(RigidbodyId rigidBodyId)
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
    for (auto& rigidbody : impl_->staticRigidBodies)
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
    for (auto& rigidbody : impl_->staticRigidBodies)
    {
        auto& body = *rigidbody.second.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }
}
}  // namespace Physics

}  // namespace GameEngine
