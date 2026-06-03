#include "BulletAdapter.h"

#include <IdPool.h>
#include <Logger/Log.h>
#include <SubscriptionManager.h>
#include <Types.h>
#include <Utils.h>
#include <Utils/Variant.h>
#include <btBulletDynamicsCommon.h>

#include <algorithm>
#include <atomic>
#include <bitset>
#include <functional>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <optional>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "CollisionResultCallback.h"
#include "Container.h"
#include "Converter.h"
#include "DebugDrawer.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/Bullet/Rigidbody.h"
#include "GameEngine/Physics/CollisionContactInfo.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Physics/PhysicsApiTypes.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "LinearMath/btVector3.h"
#include "MeshShape.h"
#include "Rigidbodies.h"

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
namespace
{
class BulletMotionState : public btMotionState
{
public:
    BulletMotionState(GameObject& go, const btVector3& offset, std::atomic_bool& isSynchronizing)
        : go{go}
        , offset(offset)
        , isSynchronizing{isSynchronizing}
    {
    }

    void getWorldTransform(btTransform& worldTrans) const override
    {
        btTransform graphicTrans;
        graphicTrans.setOrigin(Convert(go.GetWorldTransform().GetPosition()));
        graphicTrans.setRotation(Convert(go.GetWorldTransform().GetRotation().value_));
        worldTrans = graphicTrans * btTransform(btQuaternion::getIdentity(), offset);
    }

    void setWorldTransform(const btTransform& worldTrans) override
    {
        isSynchronizing   = true;
        auto graphicTrans = worldTrans * btTransform(btQuaternion::getIdentity(), offset).inverse();
        go.SetWorldPositionRotation(Convert(graphicTrans.getOrigin()), Convert(graphicTrans.getRotation()));
        isSynchronizing = false;
    }

private:
    GameObject& go;
    btVector3 offset;
    std::atomic_bool& isSynchronizing;
};
}  // namespace

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
    impl_->rigidbodies.foreach ([&](auto, auto& body) { btDynamicWorld->removeRigidBody(body.btRigidbody_.get()); });
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
    auto id = impl_->shapes_.insert(std::make_unique<Shape>(std::make_unique<btBoxShape>(Convert(size)),
                                                            Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f))));
    return id;
}

ShapeId BulletAdapter::CreateCylinderColider(const PositionOffset& positionOffset, const Scale& scale, const Size& size)
{
    auto id = impl_->shapes_.insert(std::make_unique<Shape>(std::make_unique<btCylinderShape>(Convert(size)),
                                                            Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f))));
    return id;
}
ShapeId BulletAdapter::CreateSphereColider(const PositionOffset& positionOffset, const Scale&, Radius radius)
{
    auto id = impl_->shapes_.insert(std::make_unique<Shape>(std::make_unique<btSphereShape>(radius),
                                                            Convert(positionOffset /*+ vec3(0.f, scale.y / 2.f, 0.f)*/)));
    return id;
}

ShapeId BulletAdapter::CreateCapsuleColider(const PositionOffset& positionOffset, const Scale& scale, Radius radius, float height)
{
    auto id = impl_->shapes_.insert(std::make_unique<Shape>(std::make_unique<btCapsuleShape>(radius, height / 2.f),
                                                            Convert(positionOffset + vec3(0.f, scale.y / 2.f, 0.f))));
    return id;
}

ShapeId BulletAdapter::CreateTerrainColider(const PositionOffset& positionOffset, const Scale& scale, const HeightMap& heightMap)
{
    auto shape = std::make_unique<Shape>();

    auto width  = static_cast<int>(heightMap.GetImage().width);
    auto height = static_cast<int>(heightMap.GetImage().height);
    std::visit(
        visitor{
            [&](const std::vector<uint8>& data)
            {
                shape->btShape_.reset(new btHeightfieldTerrainShape(width, height, &data[0], 1.f, heightMap.GetMinimumHeight(),
                                                                    heightMap.GetMaximumHeight(), 1, PHY_UCHAR, false));
            },
            [&](const std::vector<float>& data)
            {
                shape->btShape_.reset(new btHeightfieldTerrainShape(width, height, &data[0], 1.f, heightMap.GetMinimumHeight(),
                                                                    heightMap.GetMaximumHeight(), 1, PHY_FLOAT, false));
            },
            [](std::monostate) { LOG_ERROR << "Height map data is not set!."; },
        },
        heightMap.GetImage().getImageData());

    float scaleX = scale.x / static_cast<float>(heightMap.GetImage().width - 1);
    float scaleY = scale.z / static_cast<float>(heightMap.GetImage().height - 1);

    shape->btShape_->setLocalScaling(btVector3(scaleX, scale.y, scaleY));

    auto offset            = heightMap.GetMaximumHeight() * scale.y - (heightMap.GetDeltaHeight() * scale.y / 2.f);
    shape->positionOffset_ = Convert(positionOffset + vec3(0, offset, 0));
    shape->isTerrainShape  = true;
    return impl_->shapes_.insert(std::move(shape));
}
ShapeId BulletAdapter::CreateMeshCollider(const PositionOffset& positionOffset, const std::vector<float>& data,
                                          const IndicesVector& indicies, const vec3& scale, bool autoOptimize)
{
    if (data.empty())
    {
        return std::nullopt;
    }

    if (not autoOptimize)
    {
        if (indicies.empty())
            return std::nullopt;

        auto btMesh = std::make_unique<btTriangleMesh>(true, false);

        for (size_t i = 0; i < indicies.size(); i += 3)
        {
            if (i + 2 < indicies.size())
            {
                auto idx0 = indicies[i];
                auto idx1 = indicies[i + 1];
                auto idx2 = indicies[i + 2];
                if (3 * idx0 + 2 < data.size() and 3 * idx1 + 2 < data.size() and 3 * idx2 + 2 < data.size())
                {
                    btVector3 v0(data[3 * idx0], data[3 * idx0 + 1], data[3 * idx0 + 2]);
                    btVector3 v1(data[3 * idx1], data[3 * idx1 + 1], data[3 * idx1 + 2]);
                    btVector3 v2(data[3 * idx2], data[3 * idx2 + 1], data[3 * idx2 + 2]);

                    btMesh->addTriangle(v0, v1, v2);
                }
            }
        }

        auto meshShape      = std::make_unique<MeshShape>(std::move(btMesh));
        auto btMeshShapePtr = meshShape->btMesh_.get();
        meshShape->btShape_ = std::make_unique<btBvhTriangleMeshShape>(btMeshShapePtr, true, true);
        meshShape->btShape_->setLocalScaling(Convert(scale));
        meshShape->positionOffset_ = Convert(positionOffset);

        return impl_->shapes_.insert(std::move(meshShape));
    }
    else
    {
        auto hullShape = std::make_unique<btConvexHullShape>();

        for (size_t i = 0; i < data.size(); i += 3)
        {
            if (i + 2 < data.size())
                hullShape->addPoint(btVector3(data[i], data[i + 1], data[i + 2]), false);
        }

        hullShape->optimizeConvexHull();
        hullShape->setLocalScaling(Convert(scale));
        hullShape->recalcLocalAabb();

        return impl_->shapes_.insert(std::make_unique<Shape>(std::move(hullShape), Convert(positionOffset)));
    }
    return {};
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
        rigidbody->btRigidbody_->setLinearVelocity(Convert(velocity));
    }
}

void BulletAdapter::ApplyImpulse(const RigidbodyId& rigidBodyId, const vec3& impulse)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        rigidbody->btRigidbody_->applyCentralImpulse(Convert(impulse));
    }
}
void BulletAdapter::IncreaseVelocityRigidbody(const RigidbodyId& rigidBodyId, const vec3& velocity)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        const auto& v = rigidbody->btRigidbody_->getLinearVelocity();
        rigidbody->btRigidbody_->setLinearVelocity(v + Convert(velocity));
    }
}
std::optional<vec3> BulletAdapter::GetVelocity(const RigidbodyId& rigidBodyId)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        return Convert(rigidbody->btRigidbody_->getLinearVelocity());
    }

    return std::nullopt;
}
void BulletAdapter::SetAngularFactor(const RigidbodyId& rigidBodyId, float value)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (compare(value, 0.f))
        {
            /* LOG TO FIX*/ LOG_ERROR << ("DISABLE_DEACTIVATION btRigidbody_ : " + std::to_string(*rigidBodyId));
            rigidbody->btRigidbody_->setActivationState(DISABLE_DEACTIVATION);
        }
        rigidbody->btRigidbody_->setAngularFactor(value);
    }
}

void BulletAdapter::SetAngularFactor(const RigidbodyId& rigidBodyId, const vec3& value)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        if (compare(value.x, 0.f) and compare(value.y, 0.f) and compare(value.z, 0.f))
        {
            /* LOG TO FIX*/ LOG_ERROR << ("DISABLE_DEACTIVATION btRigidbody_ : " + std::to_string(*rigidBodyId));
            rigidbody->btRigidbody_->setActivationState(DISABLE_DEACTIVATION);
        }
        rigidbody->btRigidbody_->setAngularFactor(Convert(value));
    }
}

std::optional<vec3> BulletAdapter::GetAngularFactor(const RigidbodyId& rigidBodyId)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        return Convert(rigidbody->btRigidbody_->getAngularFactor());
    }
    return std::nullopt;
}
void BulletAdapter::RemoveRigidBodyImpl(const RigidbodyId& rigidBodyId)
{
    if (not rigidBodyId)
    {
        LOG_ERROR << "Ivalid rigidbody: " << rigidBodyId;
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
                LOG_ERROR << "Ivalid shapeId: " << shapeId;
                return;
            }

            if (impl_->rigidbodies.get([&shapeId](const auto& pair) -> bool { return shapeId.value() == pair.second.shapeId; }) !=
                nullptr)
            {
                LOG_WARN << "Shape removal without deleting rigidbody!";
            }

            std::unordered_map<uint32, Rigidbody>::iterator iter;
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
                // rigidbody->btRigidbody_->translate(Convert(vector));
                auto o = rigidbody->btRigidbody_->getWorldTransform().getOrigin();
                rigidbody->btRigidbody_->getWorldTransform().setOrigin(o + Convert(vector));
                btDynamicWorld->updateSingleAabb(rigidbody->btRigidbody_.get());
            }
        });
}

void BulletAdapter::SetRigidbodyScale(const RigidbodyId& rigidBodyId, const vec3& position)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        auto maybeShape = impl_->shapes_.get(rigidbody->shapeId);
        if (maybeShape)
        {
            (*maybeShape)->btShape_->setLocalScaling(Convert(position));
        }

        rigidbody->btRigidbody_->getWorldTransform().setOrigin(Convert(position));
        btDynamicWorld->updateSingleAabb(rigidbody->btRigidbody_.get());
    }
}

void BulletAdapter::SetShapeScale(const ShapeId& shapeId, const vec3& position)
{
    if (not shapeId)
    {
        LOG_ERROR << "Invalid shapeId";
        return;
    }

    auto maybeShape = impl_->shapes_.get(*shapeId);
    if (maybeShape)
    {
        (*maybeShape)->btShape_->setLocalScaling(Convert(position));
    }
}

std::optional<Quaternion> BulletAdapter::GetRotation(const RigidbodyId& rigidBodyId) const
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
        return Convert(rigidbody->btRigidbody_->getWorldTransform().getRotation());
    }
    return std::nullopt;
}

std::optional<common::Transform> BulletAdapter::GetTransfrom(const RigidbodyId& rigidBodyId) const
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
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
        auto& body = *rigidbody->btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
    }
}
void BulletAdapter::disableVisualizatedRigidbody(const RigidbodyId& rigidBodyId)
{
    if (auto rigidbody = impl_->rigidbodies.get(rigidBodyId))
    {
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
    collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
    btDispacher            = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
    btBroadPhase           = std::make_unique<btDbvtBroadphase>();
    btSolver               = std::make_unique<btSequentialImpulseConstraintSolver>();
    btDynamicWorld         = std::make_unique<btDiscreteDynamicsWorld>(btDispacher.get(), btBroadPhase.get(), btSolver.get(),
                                                                       collisionConfiguration.get());
    btDynamicWorld->setGravity(btVector3(0, -10, 0));

    bulletDebugDrawer_ = std::make_unique<BulletDebugDrawer>();
    bulletDebugDrawer_->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    btDynamicWorld->setDebugDrawer(bulletDebugDrawer_.get());
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
            auto& [rigidbody, collisionDetection] = sub;
            auto result                           = contactTest(*rigidbody);
            if (not collisionDetection.ignoredList.empty())
            {
                result = Utils::Filter(
                    result,
                    [&ignoredList = collisionDetection.ignoredList,
                     predicate    = collisionDetection.predicate](const auto& collisionInfo)
                    {
                        auto iter1 = std::find_if(
                            ignoredList.begin(), ignoredList.end(), [&collisionInfo](auto ignoredRbId)
                            { return ignoredRbId == collisionInfo.rigidbodyId1 or ignoredRbId == collisionInfo.rigidbodyId2; });
                        auto result = (iter1 != ignoredList.end());
                        if (result and predicate)
                        {
                            return predicate(collisionInfo);
                        }
                        return result;
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
