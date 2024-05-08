#include "BulletAdapter.h"

#include <Logger/Log.h>

#include <algorithm>
#include <unordered_map>

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "Converter.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "MeshShape.h"
#include "Utils.h"
#include "Utils/Variant.h"

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
namespace
{
const float TRANSFROM_CHANGED_EPSILON = std::numeric_limits<float>::epsilon();
}  // namespace

// bool colissionCallbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* , int partId0, int index0,
//                           const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
//{
//    //  ((bulletObject*)obj1->getUserPointer())->hit=true;
//    //  ((bulletObject*)obj2->getUserPointer())->hit=true;
//    DEBUG_LOG("collision");
//    return false;
//}

struct BulletAdapter::Pimpl
{
    std::mutex worldMutex_;
};

BulletAdapter::BulletAdapter()
    : simualtePhysics_(true)
{
    createWorld();
    impl_ = std::make_unique<BulletAdapter::Pimpl>();
}
BulletAdapter::~BulletAdapter()
{
    DEBUG_LOG("destructor");
    for (const auto& [id, body] : rigidBodies)
        btDynamicWorld->removeRigidBody(body.btRigidbody_.get());
    for (const auto& [id, body] : staticRigidBodies)
        btDynamicWorld->removeRigidBody(body.btRigidbody_.get());
}
void BulletAdapter::EnableSimulation()
{
    simualtePhysics_ = true;
}
void BulletAdapter::Simulate(float deltaTime)
{
    if (simualtePhysics_)
    {
        std::lock_guard<std::mutex> lk(impl_->worldMutex_);
        btDynamicWorld->stepSimulation(deltaTime, 1, deltaTime);

        for (auto& [id, rigidbody] : rigidBodies)
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
    bulletDebugDrawer_->clear();
    {
        btDynamicWorld->debugDrawWorld();
    }
    return bulletDebugDrawer_->getMesh();
}
void BulletAdapter::DisableSimulation()
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    simualtePhysics_ = false;
}
ShapeId BulletAdapter::CreateBoxColider(const PositionOffset& positionOffset, const Scale& scale, const Size& size)
{
    DEBUG_LOG(std::to_string(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)));
    auto id = idPool_.getId();
    shapes_.insert({id, std::make_unique<Shape>(std::make_unique<btBoxShape>(Convert(size)),
                                                Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)))});
    return id;
}
ShapeId BulletAdapter::CreateSphereColider(const PositionOffset& positionOffset, const Scale&, Radius radius)
{
    auto id = idPool_.getId();
    shapes_.insert({id, std::make_unique<Shape>(std::make_unique<btSphereShape>(radius),
                                                Convert(positionOffset /*+ vec3(0.f, scale.y / 2.f, 0.f)*/))});
    return id;
}

ShapeId BulletAdapter::CreateCapsuleColider(const PositionOffset& positionOffset, const Scale& scale, Radius radius,
                                            float height)
{
    auto id = idPool_.getId();
    shapes_.insert({id, std::make_unique<Shape>(std::make_unique<btCapsuleShape>(radius, height / 2.f),
                                                Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f)))});

    return id;
}

ShapeId BulletAdapter::CreateTerrainColider(const PositionOffset& positionOffset, const Scale& scale,
                                            const HeightMap& heightMap)
{
    auto id = idPool_.getId();
    shapes_.insert({id, std::make_unique<Shape>()});
    auto& shape = *shapes_.at(id);

    auto width  = static_cast<int>(heightMap.GetImage().width);
    auto height = static_cast<int>(heightMap.GetImage().height);
    std::visit(
        visitor{
            [&](const std::vector<uint8>& data)
            {
                shape.btShape_.reset(new btHeightfieldTerrainShape(width, height, &data[0], 1.f,
                                                                   heightMap.GetMinimumHeight(),
                                                                   heightMap.GetMaximumHeight(), 1, PHY_UCHAR, false));
            },
            [&](const std::vector<float>& data)
            {
                shape.btShape_.reset(new btHeightfieldTerrainShape(width, height, &data[0], 1.f,
                                                                   heightMap.GetMinimumHeight(),
                                                                   heightMap.GetMaximumHeight(), 1, PHY_FLOAT, false));
            },
            [](std::monostate) { ERROR_LOG("Height map data is not set!."); },
        },
        heightMap.GetImage().getImageData());

    float scaleX = scale.x / static_cast<float>(heightMap.GetImage().width - 1);
    float scaleY = scale.z / static_cast<float>(heightMap.GetImage().height - 1);

    shape.btShape_->setLocalScaling(btVector3(scaleX, scale.y, scaleY));

    auto offset           = heightMap.GetMaximumHeight() * scale.y - (heightMap.GetDeltaHeight() * scale.y / 2.f);
    shape.positionOffset_ = Convert(positionOffset + vec3(0, offset, 0));

    return id;
}
ShapeId BulletAdapter::CreateMeshCollider(const PositionOffset& positionOffset, const std::vector<float>& data,
                                          const IndicesVector& indicies, const vec3& scale, bool autoOptimize)
{
    if (data.empty() or indicies.empty())
    {
        return std::nullopt;
    }

    auto id = idPool_.getId();
    if (not autoOptimize)
    {
        auto meshShape = std::make_unique<MeshShape>(std::make_unique<btTriangleMesh>(true, false));
        auto btMesh    = meshShape->btMesh_.get();

        for (size_t i = 0; i < indicies.size(); i += 3)
        {
            auto i1 = 3 * static_cast<size_t>(indicies[i]);
            auto i2 = 3 * static_cast<size_t>(indicies[i + 1]);
            auto i3 = 3 * static_cast<size_t>(indicies[i + 2]);

            btVector3 v0(data[i1], data[i1 + 1], data[i1 + 2]);
            btVector3 v1(data[i2], data[i2 + 1], data[i2 + 2]);
            btVector3 v2(data[i3], data[i3 + 1], data[i3 + 2]);
            btMesh->addTriangle(v0, v1, v2);
        }

        meshShape->btShape_ = std::make_unique<btBvhTriangleMeshShape>(btMesh, true, true);
        meshShape->btShape_->setLocalScaling(Convert(scale));
        meshShape->positionOffset_ = Convert(positionOffset);

        shapes_.insert({id, std::move(meshShape)});
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

        shapes_.insert({id, std::make_unique<Shape>(std::move(hullShape), Convert(positionOffset))});
    }

    return id;
}

RigidbodyId BulletAdapter::CreateRigidbody(const ShapeId& shapeId, GameObject& gameObject, float mass, bool isStatic,
                                           bool& isUpdating)
{
    if (not shapeId)
    {
        ERROR_LOG("Invalid shape id");
        return std::nullopt;
    }

    std::lock_guard<std::mutex> lk(impl_->worldMutex_);

    auto shapeIter = shapes_.find(*shapeId);
    if (shapeIter == shapes_.end() or not shapeIter->second)
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

    Rigidbody body{std::make_unique<btRigidBody>(cInfo), gameObject, shape.positionOffset_, isUpdating, *shapeId};
    body.btRigidbody_->setCollisionFlags(body.btRigidbody_->getCollisionFlags() | flags);
    body.btRigidbody_->setFriction(1);

    return addRigidbody(isStatic ? staticRigidBodies : rigidBodies, std::move(body));
}
void BulletAdapter::SetVelocityRigidbody(const RigidbodyId& rigidBodyId, const vec3& velocity)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);

    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        rigidbody->btRigidbody_->setLinearVelocity(Convert(velocity));
    }
}

void BulletAdapter::ApplyImpulse(const RigidbodyId& rigidBodyId, const vec3& impulse)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);

    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        rigidbody->btRigidbody_->applyCentralImpulse(Convert(impulse));
    }
}
void BulletAdapter::IncreaseVelocityRigidbody(const RigidbodyId& rigidBodyId, const vec3& velocity)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        const auto& v = rigidbody->btRigidbody_->getLinearVelocity();
        rigidbody->btRigidbody_->setLinearVelocity(v + Convert(velocity));
    }
}
std::optional<vec3> BulletAdapter::GetVelocity(const RigidbodyId& rigidBodyId)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        return Convert(rigidbody->btRigidbody_->getLinearVelocity());
    }

    return std::nullopt;
}
void BulletAdapter::SetAngularFactor(const RigidbodyId& rigidBodyId, float value)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        if (compare(value, 0.f))
        {
            DEBUG_LOG("DISABLE_DEACTIVATION btRigidbody_ : " + std::to_string(*rigidBodyId));
            rigidbody->btRigidbody_->setActivationState(DISABLE_DEACTIVATION);
        }
        rigidbody->btRigidbody_->setAngularFactor(value);
    }
}

void BulletAdapter::SetAngularFactor(const RigidbodyId& rigidBodyId, const vec3& value)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        if (compare(value.x, 0.f) and compare(value.y, 0.f) and compare(value.z, 0.f))
        {
            DEBUG_LOG("DISABLE_DEACTIVATION btRigidbody_ : " + std::to_string(*rigidBodyId));
            rigidbody->btRigidbody_->setActivationState(DISABLE_DEACTIVATION);
        }
        rigidbody->btRigidbody_->setAngularFactor(Convert(value));
    }
}

std::optional<vec3> BulletAdapter::GetAngularFactor(const RigidbodyId& rigidBodyId)
{
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        return Convert(rigidbody->btRigidbody_->getAngularFactor());
    }
    return std::nullopt;
}
void BulletAdapter::RemoveRigidBody(const RigidbodyId& rigidBodyId)
{
    if (not rigidBodyId)
    {
        ERROR_LOG("Ivalid rigidbody");
        return;
    }

    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    auto dynamicRigidBodiesIter = rigidBodies.find(*rigidBodyId);

    if (dynamicRigidBodiesIter != rigidBodies.end())
    {
        clearRigidbody(dynamicRigidBodiesIter->second);
        rigidBodies.erase(dynamicRigidBodiesIter);
        idPool_.releaseId(*rigidBodyId);
        return;
    }

    auto staticRigidBodyIter = staticRigidBodies.find(*rigidBodyId);
    if (staticRigidBodyIter != staticRigidBodies.end())
    {
        clearRigidbody(staticRigidBodyIter->second);
        staticRigidBodies.erase(staticRigidBodyIter);
        idPool_.releaseId(*rigidBodyId);
        return;
    }

    ERROR_LOG("Rigidbody not found " + std::to_string(*rigidBodyId));
}
void BulletAdapter::RemoveShape(const ShapeId& shapeId)
{
    if (not shapeId)
    {
        ERROR_LOG("Ivalid shapeId");
        return;
    }

    std::lock_guard<std::mutex> lk(impl_->worldMutex_);

    std::unordered_map<uint32, Rigidbody>::iterator iter;

    iter = std::find_if(rigidBodies.begin(), rigidBodies.end(),
                        [id = *shapeId](auto& pair) { return pair.second.shapeId == id; });

    if (iter != rigidBodies.end())
    {
        iter->second.shapeOwner = true;
    }
    else
    {
        iter = std::find_if(staticRigidBodies.begin(), staticRigidBodies.end(),
                            [id = *shapeId](auto& pair) { return pair.second.shapeId == id; });

        if (iter != staticRigidBodies.end())
        {
            iter->second.shapeOwner = true;
        }
    }

    auto shapeIter = shapes_.find(*shapeId);
    if (shapeIter != shapes_.end())
    {
        shapes_.erase(*shapeId);
        idPool_.releaseId(*shapeId);
    }
}
void BulletAdapter::SetRotation(const RigidbodyId& rigidBodyId, const vec3& rotation)
{
    btQuaternion qt;
    qt.setEuler(rotation.y, rotation.x, rotation.z);
    auto iter = rigidBodies.find(*rigidBodyId);
    if (iter != rigidBodies.end())
    {
        iter->second.btRigidbody_->getWorldTransform().setRotation(qt);
    }
}
void BulletAdapter::SetRotation(const RigidbodyId& rigidBodyId, const Quaternion& rotation)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        rigidbody->btRigidbody_->getWorldTransform().setRotation(Convert(rotation));
    }
}
void BulletAdapter::SetPosition(const RigidbodyId& rigidBodyId, const vec3& position)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        rigidbody->btRigidbody_->getWorldTransform().setOrigin(Convert(position));
    }
}

void BulletAdapter::SetRigidbodyScale(const RigidbodyId& rigidBodyId, const vec3& position)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        auto shapeIter = shapes_.find(rigidbody->shapeId);
        if (shapeIter != shapes_.end())
        {
            shapeIter->second->btShape_->setLocalScaling(Convert(position));
        }

        rigidbody->btRigidbody_->getWorldTransform().setOrigin(Convert(position));
    }
}

void BulletAdapter::SetShapeScale(const ShapeId& shapeId, const vec3& position)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);

    if (not shapeId)
    {
        ERROR_LOG("Invalid shapeId");
        return;
    }

    auto sIter = shapes_.find(*shapeId);
    if (sIter != shapes_.end())
    {
        sIter->second->btShape_->setLocalScaling(Convert(position));
    }
}

std::optional<Quaternion> BulletAdapter::GetRotation(const RigidbodyId& rigidBodyId) const
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        return Convert(rigidbody->btRigidbody_->getWorldTransform().getRotation());
    }
    return std::nullopt;
}

std::optional<common::Transform> BulletAdapter::GetTransfrom(const RigidbodyId& rigidBodyId) const
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        return Convert(rigidbody->btRigidbody_->getWorldTransform());
    }
    return std::nullopt;
}
std::optional<RayHit> BulletAdapter::RayTest(const vec3& from, const vec3& to) const
{
    btVector3 btFrom = Convert(from);
    btVector3 btTo   = Convert(to);
    btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

    btDynamicWorld->rayTest(btFrom, btTo, res);

    if (res.hasHit())
    {
        return RayHit{Convert(res.m_hitPointWorld), Convert(res.m_hitNormalWorld)};
    }

    return std::nullopt;
}
void BulletAdapter::setVisualizatedRigidbody(const RigidbodyId& rigidBodyId)
{
    std::lock_guard<std::mutex> lk(impl_->worldMutex_);
    if (auto rigidbody = getRigidbody(rigidBodyId))
    {
        disableVisualizationForAllRigidbodys();
        auto& body = *rigidbody->btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
    }
}
void BulletAdapter::enableVisualizationForAllRigidbodys()
{
    for (auto& rigidbody : rigidBodies)
    {
        auto& body = *rigidbody.second.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
    }
    for (auto& rigidbody : staticRigidBodies)
    {
        auto& body = *rigidbody.second.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
    }
}
void BulletAdapter::disableVisualizationForAllRigidbodys()
{
    for (auto& rigidbody : rigidBodies)
    {
        auto& body = *rigidbody.second.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }
    for (auto& rigidbody : staticRigidBodies)
    {
        auto& body = *rigidbody.second.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }
}
void BulletAdapter::createWorld()
{
    collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
    btDispacher            = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
    btBroadPhase           = std::make_unique<btDbvtBroadphase>();
    btSolver               = std::make_unique<btSequentialImpulseConstraintSolver>();
    btDynamicWorld = std::make_unique<btDiscreteDynamicsWorld>(btDispacher.get(), btBroadPhase.get(), btSolver.get(),
                                                               collisionConfiguration.get());
    btDynamicWorld->setGravity(btVector3(0, -10, 0));

    bulletDebugDrawer_ = std::make_unique<BulletDebugDrawer>();
    bulletDebugDrawer_->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    btDynamicWorld->setDebugDrawer(bulletDebugDrawer_.get());
}
RigidbodyId BulletAdapter::addRigidbody(std::unordered_map<uint32, Rigidbody>& target, Rigidbody newBody)
{
    auto rigidBodyId = idPool_.getId();
    target.insert({rigidBodyId, std::move(newBody)});
    auto& body = target.at(rigidBodyId).btRigidbody_;
    body->setUserIndex(-1);

    btDynamicWorld->addRigidBody(body.get());
    btDynamicWorld->updateSingleAabb(body.get());
    return rigidBodyId;
}
Rigidbody* BulletAdapter::getRigidbody(const RigidbodyId& rigidbodyId)
{
    if (not rigidbodyId)
    {
        ERROR_LOG("Ivalid rigidbody");
        return nullptr;
    }

    auto rIter = rigidBodies.find(*rigidbodyId);
    if (rIter != rigidBodies.end())
    {
        return &rIter->second;
    }

    auto sIter = staticRigidBodies.find(*rigidbodyId);
    if (sIter != staticRigidBodies.end())
    {
        return &sIter->second;
    }

    ERROR_LOG("Rigidbody not found " + std::to_string(*rigidbodyId));

    return nullptr;
}

const Rigidbody* BulletAdapter::getRigidbody(const RigidbodyId& rigidbodyId) const
{
    if (not rigidbodyId)
    {
        ERROR_LOG("Ivalid rigidbody");
        return nullptr;
    }

    auto rIter = rigidBodies.find(*rigidbodyId);
    if (rIter != rigidBodies.end())
    {
        return &rIter->second;
    }

    auto sIter = staticRigidBodies.find(*rigidbodyId);
    if (sIter != staticRigidBodies.end())
    {
        return &sIter->second;
    }

    ERROR_LOG("Rigidbody not found " + std::to_string(*rigidbodyId));
    return nullptr;
}

void BulletAdapter::clearRigidbody(const Rigidbody& rigidbody)
{
    btDynamicWorld->removeRigidBody(rigidbody.btRigidbody_.get());

    if (rigidbody.shapeOwner)
    {
        auto shapeId = rigidbody.shapeId;

        auto shapeIter = shapes_.find(shapeId);
        if (shapeIter != shapes_.end())
        {
            shapes_.erase(shapeId);
            idPool_.releaseId(shapeId);
        }
    }
}
}  // namespace Bullet

}  // namespace Physics
}  // namespace GameEngine
