#include "BulletAdapter.h"

#include <Logger/Log.h>

#include <algorithm>
#include <unordered_map>

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "Container.h"
#include "Converter.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "MeshShape.h"
#include "Rigidbodies.h"
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

struct BulletAdapter::Pimpl
{
    Pimpl()
        : shapes_{shapesIdPool}
        , collisionContactInfoSub_{collisionContactInfoSubIdPool_}
    {
    }
    Rigidbodies rigidbodies;

    Utils::IdPool shapesIdPool;
    Container<std::unique_ptr<Shape>> shapes_;

    Utils::IdPool collisionContactInfoSubIdPool_;
    Container<std::pair<RigidbodyId, CollisionResultCallback>> collisionContactInfoSub_;

    bool visualizationForAllObjectEnabled{false};
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
    impl_->rigidbodies.foreach ([&](auto& body) { btDynamicWorld->removeRigidBody(body.btRigidbody_.get()); });
    impl_->rigidbodies.clear();
}
void BulletAdapter::EnableSimulation()
{
    simualtePhysics_ = true;
}

void BulletAdapter::Simulate(float deltaTime)
{
    executeTasks();

    if (simualtePhysics_)
    {
        btDynamicWorld->stepSimulation(deltaTime, 1, deltaTime);

        impl_->rigidbodies.dynamic_.foreach (
            [](auto& rigidbody)
            {
                auto rotatedOffset =
                    Convert(rigidbody.btRigidbody_->getWorldTransform().getRotation()) * Convert(rigidbody.positionOffset_);

                auto newPosition = Convert(rigidbody.btRigidbody_->getWorldTransform().getOrigin() - Convert(rotatedOffset));

                Quaternion newRotation = Convert(rigidbody.btRigidbody_->getWorldTransform().getRotation());

                auto l1 = glm::length(rigidbody.gameObject.GetWorldTransform().GetPosition() - newPosition);
                auto l2 = glm::length(rigidbody.gameObject.GetWorldTransform().GetRotation().value_ - newRotation);

                if (l1 > TRANSFROM_CHANGED_EPSILON or l2 > TRANSFROM_CHANGED_EPSILON)
                {
                    rigidbody.isUpdating_ = true;
                    rigidbody.gameObject.SetWorldPositionRotation(newPosition, newRotation);
                    rigidbody.isUpdating_ = false;
                }
            });

        impl_->collisionContactInfoSub_.foreach (
            [this](auto& sub)
            {
                auto& [rigidbodyId, callback] = sub;
                if (auto rigidbody = impl_->rigidbodies.get(rigidbodyId))
                {
                    if (rigidbody)
                    {
                        btDynamicWorld->contactTest(&*rigidbody->btRigidbody_, callback);
                    }
                }
            });
    }
}
const GraphicsApi::LineMesh& BulletAdapter::DebugDraw()
{
    bulletDebugDrawer_->clear();
    {
        btDynamicWorld->debugDrawWorld();
    }
    return bulletDebugDrawer_->getMesh();
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
            [](std::monostate) { ERROR_LOG("Height map data is not set!."); },
        },
        heightMap.GetImage().getImageData());

    float scaleX = scale.x / static_cast<float>(heightMap.GetImage().width - 1);
    float scaleY = scale.z / static_cast<float>(heightMap.GetImage().height - 1);

    shape->btShape_->setLocalScaling(btVector3(scaleX, scale.y, scaleY));

    auto offset            = heightMap.GetMaximumHeight() * scale.y - (heightMap.GetDeltaHeight() * scale.y / 2.f);
    shape->positionOffset_ = Convert(positionOffset + vec3(0, offset, 0));

    return impl_->shapes_.insert(std::move(shape));
}
ShapeId BulletAdapter::CreateMeshCollider(const PositionOffset& positionOffset, const std::vector<float>& data,
                                          const IndicesVector& indicies, const vec3& scale, bool autoOptimize)
{
    if (data.empty() or indicies.empty())
    {
        return std::nullopt;
    }

    if (not autoOptimize)
    {
        auto meshShape = std::make_unique<MeshShape>(std::make_unique<btTriangleMesh>(true, false));
        auto btMesh    = meshShape->btMesh_.get();

        for (size_t i = 0; i < indicies.size(); i += 3)
        {
            auto i1 = 3 * static_cast<size_t>(indicies[i]);
            auto i2 = 3 * static_cast<size_t>(indicies[i + 1]);
            auto i3 = 3 * static_cast<size_t>(indicies[i + 2]);

            if (i1 + 2 < data.size() and i2 + 2 < data.size() and i3 + 2 < data.size())
            {
                btVector3 v0(data[i1], data[i1 + 1], data[i1 + 2]);
                btVector3 v1(data[i2], data[i2 + 1], data[i2 + 2]);
                btVector3 v2(data[i3], data[i3 + 1], data[i3 + 2]);
                btMesh->addTriangle(v0, v1, v2);
            }
            else
            {
                WARNING_LOG("Out of range");
            }
        }

        meshShape->btShape_ = std::make_unique<btBvhTriangleMeshShape>(btMesh, true, true);
        meshShape->btShape_->setLocalScaling(Convert(scale));
        meshShape->positionOffset_ = Convert(positionOffset);

        return impl_->shapes_.insert(std::move(meshShape));
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

        return impl_->shapes_.insert(std::make_unique<Shape>(std::move(hullShape), Convert(positionOffset)));
    }
    return {};
}

RigidbodyId BulletAdapter::CreateRigidbody(const ShapeId& shapeId, GameObject& gameObject, const RigidbodyProperties& properties,
                                           float mass, bool& isUpdating)
{
    if (not shapeId)
    {
        ERROR_LOG("Invalid shape id");
        return std::nullopt;
    }

    auto maybeShape = impl_->shapes_.get(*shapeId);
    if (not maybeShape)
    {
        ERROR_LOG("Shape not found");
        return 0;
    }

    auto& shape = **maybeShape;

    btCollisionShape* btShape = shape.btShape_.get();
    btAssert((!btShape || btShape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    btVector3 localInertia(0, 0, 0);

    int flags = impl_->visualizationForAllObjectEnabled ? 0 : btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    bool isStatic{false};
    if (not shape.dynamicShapeAllowed_)
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
                DEBUG_LOG("NoContactResponse for: " + gameObject.GetName());
                flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
                break;
        }
    }
    if (not isStatic)
    {
        btShape->calculateLocalInertia(mass, localInertia);
    }

    auto myMotionState = new btDefaultMotionState(Convert(gameObject.GetWorldTransform(), Convert(shape.positionOffset_)));
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape.btShape_.get(), localInertia);

    Rigidbody body{std::make_unique<btRigidBody>(cInfo), gameObject, shape.positionOffset_, isUpdating, *shapeId};
    body.btRigidbody_->setCollisionFlags(body.btRigidbody_->getCollisionFlags() | flags);
    body.btRigidbody_->setFriction(1);
    body.shapeOwner = true;
    btDynamicWorld->addRigidBody(body.btRigidbody_.get());
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
            DEBUG_LOG("DISABLE_DEACTIVATION btRigidbody_ : " + std::to_string(*rigidBodyId));
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
            DEBUG_LOG("DISABLE_DEACTIVATION btRigidbody_ : " + std::to_string(*rigidBodyId));
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
        ERROR_LOG("Ivalid rigidbody");
        return;
    }

    auto collisionContactInfoSub = impl_->collisionContactInfoSub_.get(
        [rigidBodyId](const auto& p)
        {
            const auto& [id, _] = p;
            return rigidBodyId == id;
        });

    if (collisionContactInfoSub)
    {
        const auto& [id, _] = (*collisionContactInfoSub);
        impl_->collisionContactInfoSub_.erase(id);
    }

    if (auto rigidBody = impl_->rigidbodies.get(*rigidBodyId))
    {
        DEBUG_LOG("removeRigidBody : " + std::to_string(rigidBodyId));
        clearRigidbody(*rigidBody);
    }
    impl_->rigidbodies.erase(*rigidBodyId);
}

void BulletAdapter::RemoveRigidBody(const RigidbodyId& id)
{
    addTask([this, id]() { RemoveRigidBodyImpl(id); });
}
void BulletAdapter::RemoveShape(const ShapeId& shapeId)
{
    if (not shapeId)
    {
        ERROR_LOG("Ivalid shapeId");
        return;
    }

    std::unordered_map<uint32, Rigidbody>::iterator iter;

    impl_->rigidbodies.foreach (
        [shapeId](auto& rigidbody)
        {
            if (rigidbody.shapeId == shapeId)
            {
                rigidbody.shapeOwner = false;
            }
        });
    impl_->shapes_.erase(*shapeId);
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
    }
}

void BulletAdapter::SetShapeScale(const ShapeId& shapeId, const vec3& position)
{
    if (not shapeId)
    {
        ERROR_LOG("Invalid shapeId");
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
        auto rId = static_cast<uint32>(res.m_collisionObject->getUserIndex());

        return RayHit{
            .pointWorld = Convert(res.m_hitPointWorld), .normalWorld = Convert(res.m_hitNormalWorld), .rigidbodyId = rId};
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

    auto action = [](auto& rigidbody)
    {
        auto& body = *rigidbody.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
    };

    impl_->rigidbodies.foreach (action);
}
void BulletAdapter::disableVisualizationForAllRigidbodys()
{
    impl_->visualizationForAllObjectEnabled = false;

    auto action = [](auto& rigidbody)
    {
        auto& body = *rigidbody.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    };

    impl_->rigidbodies.foreach (action);
}

CollisionSubId BulletAdapter::setCollisionCallback(const RigidbodyId& rigidBodyId,
                                                   std::function<void(const CollisionContactInfo&)> callback)
{
    if (impl_->rigidbodies.get(rigidBodyId))
    {
        return impl_->collisionContactInfoSub_.insert({*rigidBodyId, CollisionResultCallback(callback)});
    }

    WARNING_LOG("rigidBodyId not found : " + std::to_string(rigidBodyId));
    return {};
}

void BulletAdapter::celarCollisionCallback(const CollisionSubId& id)
{
    addTask(
        [this, id]()
        {
            if (id)
            {
                impl_->collisionContactInfoSub_.erase(*id);
            }
        });
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

    if (rigidbody.shapeOwner)
    {
        impl_->shapes_.erase(rigidbody.shapeId);
    }
}

void BulletAdapter::addTask(Task::Action action)
{
    std::lock_guard<std::mutex> lk(tasksMutex);
    tasks.emplace_back(action);
}

void BulletAdapter::executeTasks()
{
    Tasks tmp;
    {
        std::lock_guard<std::mutex> lk(tasksMutex);
        tmp = std::move(tasks);
    }

    for (auto& task : tmp)
    {
        task.execute();
    }
}
}  // namespace Bullet

}  // namespace Physics
}  // namespace GameEngine
