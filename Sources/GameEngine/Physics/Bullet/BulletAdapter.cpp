#include "BulletAdapter.h"
#include "ShapeFactory.h"
#include "GameEngine/Physics/CollisionContactInfo.h"

#include <IdPool.h>
#include <Logger/Log.h>
#include <SubscriptionManager.h>
#include <Types.h>
#include <Utils.h>
#include <Utils/Variant.h>
#include <btBulletDynamicsCommon.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <optional>
#include <vector>

#include "CollisionResultCallback.h"
#include "Container.h"
#include "Converter.h"
#include "DebugDrawer.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/Bullet/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Physics/PhysicsApiTypes.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "LinearMath/btVector3.h"
#include "Rigidbodies.h"
#include "BulletMotionState.h"

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
struct BulletAdapter::Pimpl
{
    Rigidbodies rigidbodies;

    Container<std::unique_ptr<Shape>> shapes_;

    Container<std::pair<Rigidbody*, CollisionDetection>> collisionContactInfoSub_;
    Utils::SubscriptionManager<std::function<void()>> rigidbodiesRemoveSubs;

    bool visualizationForAllObjectEnabled{false};
};

BulletAdapter::BulletAdapter()
    : simualtePhysics_(true)
{
    createWorld();
    impl_ = std::make_unique<BulletAdapter::Pimpl>();

    tasks.reserve(1024);
    processing.reserve(1024);

    terrainShapeVisualizationConfigParamSubId =
        EngineConf.debugParams.visualizationTerrainShapeInPhysicsDebugDraw.subscribeForChange(
            [this]()
            {
                if (impl_->visualizationForAllObjectEnabled)
                {
                    disableVisualizationForAllRigidbodys();
                    enableVisualizationForAllRigidbodys();
                }
            });
}
BulletAdapter::~BulletAdapter()
{
    LOG_DEBUG << "destructor";

    if (btDynamicWorld)
    {
        impl_->rigidbodies.foreach (
            [&](auto, auto& body)
            {
                if (body.btRigidbody_)
                {
                    btDynamicWorld->removeRigidBody(body.btRigidbody_.get());
                }
            });
    }
    impl_->rigidbodies.clear();

    EngineConf.debugParams.visualizationTerrainShapeInPhysicsDebugDraw.unsubscribe(terrainShapeVisualizationConfigParamSubId);
}
void BulletAdapter::EnableSimulation()
{
    simualtePhysics_ = true;
}

void BulletAdapter::Simulate(float deltaTime)
{
    executeTasks();

    if (not simualtePhysics_)
        return;

    stepSimulation(deltaTime);
    checkCollisions();
}
void BulletAdapter::DisableSimulation()
{
    simualtePhysics_ = false;
}
ShapeId BulletAdapter::CreateBoxColider(const PositionOffset& positionOffset, const Scale& scale, const Size& size)
{
    return impl_->shapes_.insert(ShapeFactory::CreateBox(positionOffset, scale, size));
}

ShapeId BulletAdapter::CreateCylinderColider(const PositionOffset& positionOffset, const Scale& scale, const Size& size)
{
    return impl_->shapes_.insert(ShapeFactory::CreateCylinder(positionOffset, scale, size));
}
ShapeId BulletAdapter::CreateSphereColider(const PositionOffset& positionOffset, const Scale& scale, Radius radius)
{
    return impl_->shapes_.insert(ShapeFactory::CreateSphere(positionOffset, scale, radius));
}

ShapeId BulletAdapter::CreateCapsuleColider(const PositionOffset& positionOffset, const Scale& scale, Radius radius, float height)
{
    return impl_->shapes_.insert(ShapeFactory::CreateCapsule(positionOffset, scale, radius, height));
}

ShapeId BulletAdapter::CreateTerrainColider(const PositionOffset& positionOffset, const Scale& scale, const HeightMap& heightMap)
{
    return impl_->shapes_.insert(ShapeFactory::CreateTerrain(positionOffset, scale, heightMap));
}
ShapeId BulletAdapter::CreateMeshCollider(const PositionOffset& positionOffset, const std::vector<float>& data,
                                          const IndicesVector& indicies, const vec3& scale, bool autoOptimize)
{
    auto result = ShapeFactory::CreateMesh(positionOffset, data, indicies, scale, autoOptimize);
    if (result)
    {
        return impl_->shapes_.insert(std::move(*result));
    }
    return std::nullopt;
}

RigidbodyId BulletAdapter::CreateRigidbody(const ShapeId& shapeId, GameObject& gameObject, CollisionGroup group,
                                           const RigidbodyProperties& properties, float mass, std::atomic_bool& isUpdating)
{
    if (not shapeId)
    {
        LOG_ERROR << "Invalid shape id";
        return std::nullopt;
    }

    auto maybeShape = impl_->shapes_.get(*shapeId);
    if (not maybeShape)
    {
        LOG_ERROR << "Shape not found";
        return std::nullopt;
    }

    auto& shape = **maybeShape;

    btCollisionShape* btShape = shape.btShape_.get();
    btAssert((!btShape || btShape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    btVector3 localInertia(0, 0, 0);

    bool isStatic{false};

    int flags = 0;

    if (!shape.dynamicShapeAllowed_)
    {
        flags |= btCollisionObject::CF_STATIC_OBJECT;
        isStatic = true;
    }

    for (const auto property : properties)
    {
        switch (property)
        {
            case RigidbodyProperty::Static:
                flags |= btCollisionObject::CF_STATIC_OBJECT;
                isStatic = true;
                break;
            case RigidbodyProperty::NoContactResponse:
                flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
                break;
        }
    }

    if (!impl_->visualizationForAllObjectEnabled)
    {
        flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    }

    if (not isStatic)
    {
        btShape->calculateLocalInertia(mass, localInertia);
    }
    else
    {
        mass = 0;
    }

    auto myMotionState = std::make_unique<BulletMotionState>(gameObject, shape.positionOffset_, isUpdating);
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState.get(), shape.btShape_.get(), localInertia);

    Rigidbody body{std::make_unique<btRigidBody>(cInfo), std::move(myMotionState), gameObject, shape.positionOffset_, *shapeId};
    body.btRigidbody_->setCollisionFlags(flags);
    body.btRigidbody_->setFriction(1);

    std::lock_guard<std::mutex> lk(dynamicWorldMutex);

    short btGroup = btBroadphaseProxy::DefaultFilter;
    short btMask  = btBroadphaseProxy::AllFilter;

    if (group != CollisionGroup::Default)
    {
        btGroup = static_cast<short>(group);
    }

    btDynamicWorld->addRigidBody(body.btRigidbody_.get(), btGroup, btMask);
    btDynamicWorld->updateSingleAabb(body.btRigidbody_.get());
    return impl_->rigidbodies.insert(std::move(body), isStatic);
}
void BulletAdapter::SetVelocityRigidbody(const RigidbodyId& rigidBodyId, const vec3& velocity)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in SetVelocityRigidbody";
            return;
        }
        rigidbody->btRigidbody_->setLinearVelocity(Convert(velocity));
    }
}

void BulletAdapter::ApplyImpulse(const RigidbodyId& rigidBodyId, const vec3& impulse)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in ApplyImpulse";
            return;
        }
        rigidbody->btRigidbody_->applyCentralImpulse(Convert(impulse));
    }
}
void BulletAdapter::IncreaseVelocityRigidbody(const RigidbodyId& rigidBodyId, const vec3& velocity)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in IncreaseVelocityRigidbody";
            return;
        }
        const auto& v = rigidbody->btRigidbody_->getLinearVelocity();
        rigidbody->btRigidbody_->setLinearVelocity(v + Convert(velocity));
    }
}
std::optional<vec3> BulletAdapter::GetVelocity(const RigidbodyId& rigidBodyId)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in GetVelocity";
            return std::nullopt;
        }
        return Convert(rigidbody->btRigidbody_->getLinearVelocity());
    }

    return std::nullopt;
}
void BulletAdapter::SetAngularFactor(const RigidbodyId& rigidBodyId, float value)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in SetAngularFactor";
            return;
        }
        if (compare(value, 0.f))
        {
            LOG_WARN << "DISABLE_DEACTIVATION btRigidbody_ : " << *rigidBodyId;
            rigidbody->btRigidbody_->setActivationState(DISABLE_DEACTIVATION);
        }
        rigidbody->btRigidbody_->setAngularFactor(value);
    }
}

void BulletAdapter::SetAngularFactor(const RigidbodyId& rigidBodyId, const vec3& value)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in SetAngularFactor";
            return;
        }
        if (compare(value.x, 0.f) and compare(value.y, 0.f) and compare(value.z, 0.f))
        {
            LOG_WARN << "DISABLE_DEACTIVATION btRigidbody_ : " << *rigidBodyId;
            rigidbody->btRigidbody_->setActivationState(DISABLE_DEACTIVATION);
        }
        rigidbody->btRigidbody_->setAngularFactor(Convert(value));
    }
}

std::optional<vec3> BulletAdapter::GetAngularFactor(const RigidbodyId& rigidBodyId)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in GetAngularFactor";
            return std::nullopt;
        }
        return Convert(rigidbody->btRigidbody_->getAngularFactor());
    }
    return std::nullopt;
}
void BulletAdapter::RemoveRigidBodyImpl(const RigidbodyId& rigidBodyId)
{
    if (not rigidBodyId)
    {
        LOG_ERROR << "Invalid rigidbody: " << rigidBodyId;
        return;
    }

    impl_->rigidbodiesRemoveSubs.notifyAndRemoveObject(*rigidBodyId);

    auto rigidbody = impl_->rigidbodies.get(rigidBodyId);
    if (rigidbody)
    {
        std::vector<IdType> subIdsToRemove;
        impl_->collisionContactInfoSub_.foreach (
            [&subIdsToRemove, rigidbody](auto subId, const auto& pair)
            {
                auto& [rigibodyPtr, _] = pair;
                if (rigibodyPtr == rigidbody)
                {
                    subIdsToRemove.push_back(subId);
                }
            });

        for (auto id : subIdsToRemove)
        {
            impl_->collisionContactInfoSub_.erase(id);
            LOG_DEBUG << "erase subscription, collisionContactInfoSub_ " << id;
        }

        LOG_DEBUG << "removeRigidBody : " << rigidBodyId;
        clearRigidbody(*rigidbody);
    }

    impl_->rigidbodies.erase(*rigidBodyId);
}

void BulletAdapter::RemoveRigidBody(const RigidbodyId& id)
{
    addTask([this, id]() { RemoveRigidBodyImpl(id); });
}
void BulletAdapter::RemoveShape(const ShapeId& shapeId)
{
    addTask(
        [this, shapeId]()
        {
            if (not shapeId)
            {
                LOG_ERROR << "Invalid shapeId: " << shapeId;
                return;
            }

            if (impl_->rigidbodies.get([&shapeId](const auto& pair) -> bool { return shapeId.value() == pair.second.shapeId; }) !=
                nullptr)
            {
                LOG_WARN << "Shape removal without deleting rigidbody!";
            }

            impl_->shapes_.erase(*shapeId);
        });
}
void BulletAdapter::SetRotation(const RigidbodyId& rigidBodyId, const vec3& rotation)
{
    btQuaternion qt;
    qt.setEuler(rotation.y, rotation.x, rotation.z);

    addTask(
        [this, rigidBodyId, qt]()
        {
            if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
            {
                if (!rigidbody->btRigidbody_)
                {
                    LOG_ERROR << "Invalid rigidbody in SetRotation";
                    return;
                }
                rigidbody->btRigidbody_->getWorldTransform().setRotation(qt);
                btDynamicWorld->updateSingleAabb(rigidbody->btRigidbody_.get());
            }
        });
}
void BulletAdapter::SetRotation(const RigidbodyId& rigidBodyId, const Quaternion& rotation)
{
    addTask(
        [this, rigidBodyId, rotation]()
        {
            if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
            {
                if (!rigidbody->btRigidbody_)
                {
                    LOG_ERROR << "Invalid rigidbody in SetRotation";
                    return;
                }
                rigidbody->btRigidbody_->getWorldTransform().setRotation(Convert(rotation));
                btDynamicWorld->updateSingleAabb(rigidbody->btRigidbody_.get());
            }
        });
}
void BulletAdapter::SetPosition(const RigidbodyId& rigidBodyId, const vec3& position)
{
    addTask(
        [this, rigidBodyId, position]()
        {
            if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
            {
                if (!rigidbody->btRigidbody_)
                {
                    LOG_ERROR << "Invalid rigidbody in SetPosition";
                    return;
                }
                rigidbody->btRigidbody_->getWorldTransform().setOrigin(Convert(position));
                btDynamicWorld->updateSingleAabb(rigidbody->btRigidbody_.get());
            }
        });
}

void BulletAdapter::Translate(const RigidbodyId& rigidBodyId, const vec3& vector)
{
    addTask(
        [this, rigidBodyId, vector]()
        {
            if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
            {
                if (!rigidbody->btRigidbody_)
                {
                    LOG_ERROR << "Invalid rigidbody in Translate";
                    return;
                }
                // rigidbody->btRigidbody_->translate(Convert(vector));
                auto o = rigidbody->btRigidbody_->getWorldTransform().getOrigin();
                rigidbody->btRigidbody_->getWorldTransform().setOrigin(o + Convert(vector));
                btDynamicWorld->updateSingleAabb(rigidbody->btRigidbody_.get());
            }
        });
}

void BulletAdapter::SetRigidbodyScale(const RigidbodyId& rigidBodyId, const vec3& scale)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        auto maybeShape = impl_->shapes_.get(rigidbody->shapeId);
        if (maybeShape)
        {
            (*maybeShape)->btShape_->setLocalScaling(Convert(scale));
        }

        btDynamicWorld->updateSingleAabb(rigidbody->btRigidbody_.get());
    }
}

void BulletAdapter::SetShapeScale(const ShapeId& shapeId, const vec3& scale)
{
    if (not shapeId)
    {
        LOG_ERROR << "Invalid shapeId";
        return;
    }

    auto maybeShape = impl_->shapes_.get(*shapeId);
    if (maybeShape)
    {
        (*maybeShape)->btShape_->setLocalScaling(Convert(scale));
    }
}

std::optional<Quaternion> BulletAdapter::GetRotation(const RigidbodyId& rigidBodyId) const
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in GetRotation";
            return std::nullopt;
        }
        return Convert(rigidbody->btRigidbody_->getWorldTransform().getRotation());
    }
    return std::nullopt;
}

std::optional<common::Transform> BulletAdapter::GetTransfrom(const RigidbodyId& rigidBodyId) const
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in GetTransform";
            return std::nullopt;
        }
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
        auto rId       = static_cast<uint32>(res.m_collisionObject->getUserIndex());
        auto rigidBody = impl_->rigidbodies.get(rId);

        return RayHit{.gameObject  = rigidBody->gameObject,
                      .pointWorld  = Convert(res.m_hitPointWorld),
                      .normalWorld = Convert(res.m_hitNormalWorld),
                      .rigidbodyId = rId};
    }

    return std::nullopt;
}
void BulletAdapter::enableVisualizatedRigidbody(const RigidbodyId& rigidBodyId)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in enableVisualizatedRigidbody";
            return;
        }
        auto& body = *rigidbody->btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
    }
}
void BulletAdapter::disableVisualizatedRigidbody(const RigidbodyId& rigidBodyId)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (!rigidbody->btRigidbody_)
        {
            LOG_ERROR << "Invalid rigidbody in disableVisualizatedRigidbody";
            return;
        }
        auto& body = *rigidbody->btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }
}
void BulletAdapter::enableVisualizationForAllRigidbodys()
{
    impl_->visualizationForAllObjectEnabled = true;

    auto action = [&](auto, auto& rigidbody)
    {
        auto& body = *rigidbody.btRigidbody_;

        if (not EngineConf.debugParams.visualizationTerrainShapeInPhysicsDebugDraw)
        {
            if (const auto& shape = impl_->shapes_.get(rigidbody.shapeId); (*shape)->isTerrainShape)
            {
                return;
            }
        }

        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
    };

    impl_->rigidbodies.foreach (action);
}
void BulletAdapter::disableVisualizationForAllRigidbodys()
{
    impl_->visualizationForAllObjectEnabled = false;

    auto action = [](auto, auto& rigidbody)
    {
        auto& body = *rigidbody.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    };

    impl_->rigidbodies.foreach (action);
}

CollisionSubId BulletAdapter::setCollisionCallback(const RigidbodyId& rigidBodyId, const CollisionDetection& collision)
{
    if (auto rb = impl_->rigidbodies.get(rigidBodyId))
    {
        return impl_->collisionContactInfoSub_.insert({rb, collision});
    }

    LOG_WARN << "rigidBodyId not found : " << rigidBodyId;
    return {};
}

void BulletAdapter::celarCollisionCallback(const CollisionSubId& id)
{
    addTask([this, id]() { impl_->collisionContactInfoSub_.erase(*id); });
}
CollisionContactInfos BulletAdapter::contactTest(const Rigidbody& rigidbody)
{
    CollisionContactInfos resultCallbackData;
    CollisionResultCallback singleCollisionCallback([&resultCallbackData](const auto& info)
                                                    { resultCallbackData.push_back(info); });
    btDynamicWorld->contactTest(&*rigidbody.btRigidbody_, singleCollisionCallback);
    return resultCallbackData;
}
void BulletAdapter::createWorld()
{
    if (btDynamicWorld)
    {
        LOG_WARN << "Physics world already initialized, skipping creation";
        return;
    }

    collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
    btDispacher            = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
    btBroadPhase           = std::make_unique<btDbvtBroadphase>();
    btSolver               = std::make_unique<btSequentialImpulseConstraintSolver>();
    btDynamicWorld         = std::make_unique<btDiscreteDynamicsWorld>(btDispacher.get(), btBroadPhase.get(), btSolver.get(),
                                                                       collisionConfiguration.get());
    if (!btDynamicWorld)
    {
        LOG_ERROR << "Failed to create physics world!";
        return;
    }

    btDynamicWorld->setGravity(btVector3(0, -10, 0));

    bulletDebugDrawer_ = std::make_unique<BulletDebugDrawer>();
    if (bulletDebugDrawer_)
    {
        bulletDebugDrawer_->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
        btDynamicWorld->setDebugDrawer(bulletDebugDrawer_.get());
    }
    else
    {
        LOG_WARN << "Failed to create debug drawer!";
    }
}

void BulletAdapter::clearRigidbody(const Rigidbody& rigidbody)
{
    btDynamicWorld->removeRigidBody(rigidbody.btRigidbody_.get());
}

IdType BulletAdapter::addTask(Task::Action action, std::optional<IdType> reusedId)
{
    std::lock_guard<std::mutex> lk(tasksMutex);
    auto id = reusedId ? *reusedId : taskIdPool_.getId();
    tasks.push_back({id, action});
    return id;
}

void BulletAdapter::removeTask(IdType id)
{
    std::lock_guard<std::mutex> lk(tasksMutex);
    auto iter = std::find_if(tasks.begin(), tasks.end(), [id](const auto& task) { return task.GetId() == id; });
    if (iter != tasks.end())
    {
        tasks.erase(iter);
    }
}

void BulletAdapter::executeTasks()
{
    {
        std::lock_guard<std::mutex> lk(tasksMutex);
        std::swap(processing, tasks);
    }

    for (auto& task : processing)
    {
        task.execute();
    }
    processing.clear();
}

std::optional<BoundingBox> BulletAdapter::getBoundingBox(const RigidbodyId& rigidBodyId) const
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        btVector3 minAABB, maxAABB;
        rigidbody->btRigidbody_->getAabb(minAABB, maxAABB);

        return BoundingBox(Convert(minAABB), Convert(maxAABB));
    }

    return std::nullopt;
}
std::vector<IdType> BulletAdapter::getObjectsInBox(const vec3& pos, const vec3& halfExtents) const
{
    auto boxShape  = btBoxShape(Convert(halfExtents));
    auto transform = btTransform();
    transform.setIdentity();
    transform.setOrigin(Convert(pos));

    auto tempObject = btCollisionObject();
    tempObject.setCollisionShape(&boxShape);
    tempObject.setWorldTransform(transform);

    struct ContactSensorCallback : public btCollisionWorld::ContactResultCallback
    {
        const btCollisionObject* m_me;
        std::vector<IdType> collidedIds;

        ContactSensorCallback(const btCollisionObject* me)
            : m_me(me)
        {
            m_collisionFilterGroup = CollisionGroup::Default;
            m_collisionFilterMask  = CollisionGroup::All ^ CollisionGroup::Terrain;
            collidedIds.reserve(16);
        }

        btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
                                 const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
        {
            auto otherObj = (colObj0Wrap->getCollisionObject() == m_me) ? colObj1Wrap->getCollisionObject()
                                                                        : colObj0Wrap->getCollisionObject();
            collidedIds.push_back(static_cast<IdType>(otherObj->getUserIndex()));
            return 0;
        }
    };

    auto callback = ContactSensorCallback(&tempObject);
    auto lk       = std::lock_guard<std::mutex>(dynamicWorldMutex);

    btDynamicWorld->contactTest(&tempObject, callback);

    std::sort(callback.collidedIds.begin(), callback.collidedIds.end());
    auto last = std::unique(callback.collidedIds.begin(), callback.collidedIds.end());
    callback.collidedIds.erase(last, callback.collidedIds.end());

    return callback.collidedIds;
}
void BulletAdapter::checkCollisions()
{
    std::lock_guard<std::mutex> lk(dynamicWorldMutex);

    std::vector<IdType> idToRemove;
    impl_->collisionContactInfoSub_.foreach (
        [this, &idToRemove](auto subId, auto& sub)
        {
            if (not sub.first)
                return;

            auto& [rigidbody, collisionDetection] = sub;
            auto result                           = contactTest(*rigidbody);

            // Utils::Filter erases elements for which the predicate returns true.
            // Always apply ignoredList / predicate when present so contact callbacks
            // (e.g. ground detection) are not polluted by unrelated collisions like walls.
            if (not collisionDetection.ignoredList.empty() or collisionDetection.predicate)
            {
                result = Utils::Filter(result,
                                       [&ignoredList = collisionDetection.ignoredList,
                                        predicate    = collisionDetection.predicate](const auto& collisionInfo)
                                       {
                                           if (not ignoredList.empty())
                                           {
                                               const bool isIgnored =
                                                   std::any_of(ignoredList.begin(), ignoredList.end(),
                                                               [&collisionInfo](auto ignoredRbId)
                                                               {
                                                                   return ignoredRbId == collisionInfo.rigidbodyId1 or
                                                                          ignoredRbId == collisionInfo.rigidbodyId2;
                                                               });
                                               if (isIgnored)
                                               {
                                                   return true;
                                               }
                                           }

                                           if (predicate)
                                           {
                                               return not predicate(collisionInfo);
                                           }

                                           return false;
                                       });
            }

            auto finalCallback = [&, cd = collisionDetection]
            {
                cd.callback(result);

                if (cd.type == CollisionDetection::Type::single)
                {
                    idToRemove.push_back(subId);
                }
            };

            switch (collisionDetection.action)
            {
                case CollisionDetection::Action::any:
                    finalCallback();
                    break;
                case CollisionDetection::Action::onEnter:
                    if (not result.empty() and not collisionDetection.lastContactState)
                    {
                        finalCallback();
                    }
                    break;
                case CollisionDetection::Action::onExit:
                    if (result.empty() and collisionDetection.lastContactState)
                    {
                        finalCallback();
                    }
                    break;
                case CollisionDetection::Action::on:
                    if (not result.empty())
                    {
                        finalCallback();
                    }
                    break;
                case CollisionDetection::Action::no:
                    if (result.empty())
                    {
                        finalCallback();
                    }
                    break;
            }
            collisionDetection.lastContactState = not result.empty();
        });

    for (auto id : idToRemove)
    {
        impl_->collisionContactInfoSub_.erase(id);
    }
}
void BulletAdapter::stepSimulation(float deltaTime)
{
    if (!btDynamicWorld)
    {
        LOG_ERROR << "Physics world not initialized!";
        return;
    }

    std::lock_guard<std::mutex> lk(dynamicWorldMutex);
    btDynamicWorld->stepSimulation(deltaTime, 1, deltaTime);
}
const GraphicsApi::LineMesh& BulletAdapter::DebugDraw(const vec3& cameraPos)
{
    bulletDebugDrawer_->clear();
    bulletDebugDrawer_->setCameraPos(cameraPos);
    {
        btDynamicWorld->debugDrawWorld();
    }
    return bulletDebugDrawer_->getMesh();
}
MaybeId BulletAdapter::subscribeForRigidbodyRemove(const RigidbodyId& id, std::function<void()> function)
{
    if (not id)
        return std::nullopt;

    return impl_->rigidbodiesRemoveSubs.subscribe(*id, function);
}
void BulletAdapter::unsubscribeForRigidbodyRemove(IdType id)
{
    impl_->rigidbodiesRemoveSubs.unsubscribe(id);
}
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
