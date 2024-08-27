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

template <class Type>
class Container
{
public:
    Container(Utils::IdPool& idPool)
        : idPool_{idPool}
    {
    }
    IdType insert(Type type)
    {
        std::lock_guard<std::mutex> lk(mutex);
        auto id = idPool_.getId();
        values.insert({id, std::move(type)});
        return id;
    }
    void erase(IdType shapeId)
    {
        std::lock_guard<std::mutex> lk(mutex);
        idPool_.releaseId(shapeId);
        values.erase(shapeId);
    }
    Type* get(IdType shapeId)
    {
        std::lock_guard<std::mutex> lk(mutex);
        auto it = values.find(shapeId);
        if (it != values.end())
        {
            auto& [_, v] = *it;
            return &v;
        }
        return nullptr;
    }
    void clear()
    {
        std::lock_guard<std::mutex> lk(mutex);
        values.clear();
    }
    void foreach (std::function<void(Type&)> f)
    {
        std::lock_guard<std::mutex> lk(mutex);
        for (auto& [_, value] : values)
        {
            f(value);
        }
    }

private:
    std::unordered_map<IdType, Type> values;
    std::mutex mutex;
    Utils::IdPool& idPool_;

};
}  // namespace

struct Rigidbodies
{

};

struct BulletAdapter::Pimpl
{
    Pimpl()
        : rigidBodies{rigidBodiesIdPool}
        , staticRigidBodies{rigidBodiesIdPool}
        , shapes_{shapesIdPool}
    {
    }
    Container<Rigidbody> rigidBodies;
    Container<Rigidbody> staticRigidBodies;
    Container<std::unique_ptr<Shape>> shapes_;
    Utils::IdPool rigidBodiesIdPool;
    Utils::IdPool shapesIdPool;

    bool visualizationForAllObjectEnabled{false};

    RigidbodyId addRigidbody(Container<Rigidbody>&, Rigidbody);
    Rigidbody* getRigidbody(const RigidbodyId&);
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
    impl_->rigidBodies.foreach ([&](auto& body) { btDynamicWorld->removeRigidBody(body.btRigidbody_.get()); });
    impl_->staticRigidBodies.foreach ([&](auto& body) { btDynamicWorld->removeRigidBody(body.btRigidbody_.get()); });

    impl_->rigidBodies.clear();
    impl_->staticRigidBodies.clear();
}
void BulletAdapter::EnableSimulation()
{
    simualtePhysics_ = true;
}

void BulletAdapter::Simulate(float deltaTime)
{
    RemoveQueuedRigidbodies();
    RemoveQueuedCollisionCallbacks();

    if (simualtePhysics_)
    {
        btDynamicWorld->stepSimulation(deltaTime, 1, deltaTime);

        impl_->rigidBodies.foreach (
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

        for (auto& [_, pair] : collisionContactInfoSub)
        {
            auto& [rigidbodyId, callback] = pair;
            if (auto rigidbody = impl_->getRigidbody(rigidbodyId))
            {
                if (rigidbody)
                {
                    btDynamicWorld->contactTest(&*rigidbody->btRigidbody_, callback);
                }
            }
        }
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
                DEBUG_LOG("nameToTypeMap_ " + gameObject.GetName());
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
    btDynamicWorld->addRigidBody(body.btRigidbody_.get());
    btDynamicWorld->updateSingleAabb(body.btRigidbody_.get());
    return impl_->addRigidbody(isStatic ? impl_->staticRigidBodies : impl_->rigidBodies, std::move(body));
}
void BulletAdapter::SetVelocityRigidbody(const RigidbodyId& rigidBodyId, const vec3& velocity)
{
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
    {
        rigidbody->btRigidbody_->setLinearVelocity(Convert(velocity));
    }
}

void BulletAdapter::ApplyImpulse(const RigidbodyId& rigidBodyId, const vec3& impulse)
{
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
    {
        rigidbody->btRigidbody_->applyCentralImpulse(Convert(impulse));
    }
}
void BulletAdapter::IncreaseVelocityRigidbody(const RigidbodyId& rigidBodyId, const vec3& velocity)
{
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
    {
        const auto& v = rigidbody->btRigidbody_->getLinearVelocity();
        rigidbody->btRigidbody_->setLinearVelocity(v + Convert(velocity));
    }
}
std::optional<vec3> BulletAdapter::GetVelocity(const RigidbodyId& rigidBodyId)
{
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
    {
        return Convert(rigidbody->btRigidbody_->getLinearVelocity());
    }

    return std::nullopt;
}
void BulletAdapter::SetAngularFactor(const RigidbodyId& rigidBodyId, float value)
{
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
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
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
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
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
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

    auto collisionContactInfoSubIter = std::find_if(collisionContactInfoSub.begin(), collisionContactInfoSub.end(),
                                                    [rigidBodyId](const auto& p) { return rigidBodyId == p.second.first; });

    if (collisionContactInfoSubIter != collisionContactInfoSub.end())
    {
        collisionContactInfoSub.erase(collisionContactInfoSubIter);
    }

    if (auto dynamicRigidBody = impl_->rigidBodies.get(*rigidBodyId))
    {
        clearRigidbody(*dynamicRigidBody);
        impl_->rigidBodies.erase(*rigidBodyId);
        return;
    }

    if (auto staticRigidBody = impl_->staticRigidBodies.get(*rigidBodyId))
    {
        clearRigidbody(*staticRigidBody);
        impl_->staticRigidBodies.erase(*rigidBodyId);
        return;
    }

    ERROR_LOG("Rigidbody not found " + std::to_string(*rigidBodyId));
}

void BulletAdapter::RemoveRigidBody(const RigidbodyId& id)
{
    std::lock_guard<std::mutex> lk(rigidbodyToRemoveMutex);
    rigidbodyToRemove.push_back(id);
}
void BulletAdapter::RemoveShape(const ShapeId& shapeId)
{
    if (not shapeId)
    {
        ERROR_LOG("Ivalid shapeId");
        return;
    }

    std::unordered_map<uint32, Rigidbody>::iterator iter;

    // to do check shapeOwner

    // iter =
    //     std::find_if(rigidBodies.begin(), rigidBodies.end(), [id = *shapeId](auto& pair) { return pair.second.shapeId == id;
    //     });

    // if (iter != rigidBodies.end())
    // {
    //     iter->second.shapeOwner = true;
    // }
    // else
    // {
    //     iter = std::find_if(staticRigidBodies.begin(), staticRigidBodies.end(),
    //                         [id = *shapeId](auto& pair) { return pair.second.shapeId == id; });

    //     if (iter != staticRigidBodies.end())
    //     {
    //         iter->second.shapeOwner = true;
    //     }
    // }

    impl_->shapes_.erase(*shapeId);
}
void BulletAdapter::SetRotation(const RigidbodyId& rigidBodyId, const vec3& rotation)
{
    btQuaternion qt;
    qt.setEuler(rotation.y, rotation.x, rotation.z);

    if (auto rigidbody = impl_->getRigidbody(*rigidBodyId))
    {
        rigidbody->btRigidbody_->getWorldTransform().setRotation(qt);
    }
}
void BulletAdapter::SetRotation(const RigidbodyId& rigidBodyId, const Quaternion& rotation)
{
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
    {
        rigidbody->btRigidbody_->getWorldTransform().setRotation(Convert(rotation));
    }
}
void BulletAdapter::SetPosition(const RigidbodyId& rigidBodyId, const vec3& position)
{
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
    {
        rigidbody->btRigidbody_->getWorldTransform().setOrigin(Convert(position));
    }
}

void BulletAdapter::SetRigidbodyScale(const RigidbodyId& rigidBodyId, const vec3& position)
{
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
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
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
    {
        return Convert(rigidbody->btRigidbody_->getWorldTransform().getRotation());
    }
    return std::nullopt;
}

std::optional<common::Transform> BulletAdapter::GetTransfrom(const RigidbodyId& rigidBodyId) const
{
    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
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
void BulletAdapter::setVisualizatedRigidbody(const RigidbodyId& rigidBodyId)
{
    disableVisualizationForAllRigidbodys();

    if (auto rigidbody = impl_->getRigidbody(rigidBodyId))
    {
        auto& body = *rigidbody->btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
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

    impl_->rigidBodies.foreach (action);
    impl_->staticRigidBodies.foreach (action);
}
void BulletAdapter::disableVisualizationForAllRigidbodys()
{
    impl_->visualizationForAllObjectEnabled = false;

    auto action = [](auto& rigidbody)
    {
        auto& body = *rigidbody.btRigidbody_;
        body.setCollisionFlags(body.getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    };

    impl_->rigidBodies.foreach (action);
    impl_->staticRigidBodies.foreach (action);
}

CollisionSubId BulletAdapter::setCollisionCallback(const RigidbodyId& rigidBodyId,
                                                   std::function<void(const CollisionContactInfo&)> callback)
{
    if (impl_->getRigidbody(rigidBodyId))
    {
        auto id = collisionContactInfoSubIdPool_.getId();
        collisionContactInfoSub.insert({id, {*rigidBodyId, CollisionResultCallback(callback)}});
        return id;
    }

    WARNING_LOG("rigidBodyId not found : " + std::to_string(rigidBodyId));
    return {};
}

void BulletAdapter::celarCollisionCallback(const CollisionSubId& id)
{
    if (id)
    {
        std::lock_guard<std::mutex> lk(collisionCallbacksToRemoveMutex);
        collisionCallbacksToRemove.push_back(id);
    }
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
RigidbodyId BulletAdapter::Pimpl::addRigidbody(Container<Rigidbody>& target, Rigidbody newBody)
{
    auto btRigidbody = newBody.btRigidbody_.get();

    auto rigidBodyId = target.insert(std::move(newBody));
    btRigidbody->setUserIndex(static_cast<int>(rigidBodyId));
    return rigidBodyId;
}
Rigidbody* BulletAdapter::Pimpl::getRigidbody(const RigidbodyId& rigidbodyId)
{
    if (not rigidbodyId)
    {
        ERROR_LOG("Ivalid rigidbody");
        return nullptr;
    }

    auto maybeRigidBody = rigidBodies.get(*rigidbodyId);
    if (maybeRigidBody)
    {
        return maybeRigidBody;
    }

    maybeRigidBody = staticRigidBodies.get(*rigidbodyId);
    if (maybeRigidBody)
    {
        return maybeRigidBody;
    }

    ERROR_LOG("Rigidbody not found " + std::to_string(*rigidbodyId));
    return nullptr;
}

void BulletAdapter::clearRigidbody(const Rigidbody& rigidbody)
{
    btDynamicWorld->removeRigidBody(rigidbody.btRigidbody_.get());

    if (rigidbody.shapeOwner)
    {
        impl_->shapes_.erase(rigidbody.shapeId);
    }
}

void BulletAdapter::RemoveQueuedRigidbodies()
{
    std::lock_guard<std::mutex> lk(rigidbodyToRemoveMutex);
    for (const auto& id : rigidbodyToRemove)
    {
        RemoveRigidBodyImpl(id);
    }
    rigidbodyToRemove.clear();
}

void BulletAdapter::RemoveQueuedCollisionCallbacks()
{
    std::lock_guard<std::mutex> lk(collisionCallbacksToRemoveMutex);

    for (const auto& id : collisionCallbacksToRemove)
    {
        collisionContactInfoSub.erase(*id);
    }
    collisionCallbacksToRemove.clear();
}
}  // namespace Bullet

}  // namespace Physics
}  // namespace GameEngine
