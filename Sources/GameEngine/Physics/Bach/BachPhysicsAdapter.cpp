#include "BachPhysicsAdapter.h"

#include <Logger/Log.h>

#include <iomanip>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <variant>

#include "Common/Transform.h"
#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "Types.h"

namespace GameEngine
{
namespace Physics
{
class Rigidbody;
struct TerrainShape;
struct CollisionData
{
    vec3 position_;
    vec3 outDirecion_;
};
struct SphehreShape
{
    vec3 positionOffset;
    float r;
};

struct MeshShape
{
};

struct BoxShape
{
};

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

typedef std::variant<TerrainShape, SphehreShape, MeshShape, BoxShape> Shape;

void collision(const SphehreShape&, const SphehreShape&)
{
}

void collision(const TerrainShape&, const SphehreShape&)
{
}

void collision(const MeshShape&, const SphehreShape&)
{
}

void collision(const BoxShape&, const SphehreShape&)
{
}

void collision(const MeshShape&, const TerrainShape&)
{
}

void collision(const BoxShape&, const TerrainShape&)
{
}

template <typename T>
void collision(const Shape& collisionShape, const T& shape)
{
    std::visit(
        overloaded{
            [&shape](const SphehreShape& arg) { collision(arg, shape); },
            [&shape](const TerrainShape& arg) { collision(arg, shape); },
            [&shape](const MeshShape& arg) { collision(arg, shape); },
            [&shape](const BoxShape& arg) { collision(arg, shape); },
        },
        collisionShape);
}

class Rigidbody
{
public:
    Rigidbody(GameObject& gameObject, float mass, bool isStatic, uint32 shapeId)
        : gameObject_(gameObject)
        , mass_(mass)
        , isStatic_(isStatic)
        , shapeId_(shapeId)
    {
    }

    vec3 velocity_;
    GameObject& gameObject_;
    float mass_;
    bool isStatic_;
    uint32 shapeId_;
};

struct TerrainShape
{
    TerrainShape(const vec2ui&, std::vector<float>*, const vec2&, float heightFactor)
        : heightFactor_(heightFactor)
    {
    }
    std::optional<CollisionData> collision(Rigidbody&)
    {
        return {};
    }
    float heightFactor_;
};

struct BachPhysicsAdapter::Pimpl
{
    Pimpl()
        : id_(0)
        , enableSimulation_(true)
        , simulationStep_(1.f)
    {
    }
    std::unordered_map<uint32, SphehreShape /*Shape*/> shapes_;
    std::unordered_map<uint32, Rigidbody> rigidbodies_;
    std::vector<TerrainShape> terrains_;
    uint32 id_;
    bool enableSimulation_;
    float simulationStep_;
};

BachPhysicsAdapter::BachPhysicsAdapter()
{
    impl_.reset(new Pimpl());
}
BachPhysicsAdapter::~BachPhysicsAdapter()
{
    DEBUG_LOG("destructor");
}
void BachPhysicsAdapter::Simulate(float)
{
    if (not impl_->enableSimulation_)
    {
        return;
    }

    for (auto& terrain : impl_->terrains_)
    {
        for (auto& rigidbody : impl_->rigidbodies_)
        {
            auto collisionData = terrain.collision(rigidbody.second);

            if (collisionData)
            {
                rigidbody.second.gameObject_.SetWorldPosition(collisionData->position_);
            }
        }
    }
}
const GraphicsApi::LineMesh& BachPhysicsAdapter::DebugDraw()
{
    return lineMesh_;
}
void BachPhysicsAdapter::EnableSimulation()
{
    impl_->enableSimulation_ = true;
}
void BachPhysicsAdapter::DisableSimulation()
{
    impl_->enableSimulation_ = false;
}
ShapeId BachPhysicsAdapter::CreateBoxColider(const vec3&, const vec3&, const vec3&)
{
    return impl_->id_++;
}

ShapeId BachPhysicsAdapter::CreateCylinderColider(const PositionOffset&, const Scale&, const Size&)
{
    return impl_->id_++;
}
ShapeId BachPhysicsAdapter::CreateSphereColider(const vec3& positionOffset, const vec3&, float radius)
{
    impl_->shapes_.insert({impl_->id_, {positionOffset, radius}});
    return impl_->id_++;
}
ShapeId BachPhysicsAdapter::CreateCapsuleColider(const vec3&, const vec3&, float, float)
{
    return uint32();
}
ShapeId BachPhysicsAdapter::CreateTerrainColider(const vec3&, const vec3&, const HeightMap&)
{
    // impl_->terrains_.emplace_back(size, data, vec3(0), hightFactor);
    return impl_->id_++;
}
ShapeId BachPhysicsAdapter::CreateMeshCollider(const vec3&, const std::vector<float>&, const IndicesVector&,
                                               const vec3&, bool)
{
    return impl_->id_++;
}
RigidbodyId BachPhysicsAdapter::CreateRigidbody(const ShapeId& shapeId, GameObject& transform,
                                                const RigidbodyProperties&, float mass, bool&)
{
    impl_->rigidbodies_.insert({impl_->id_, Rigidbody(transform, mass, false, *shapeId)});
    return impl_->id_++;
}
void BachPhysicsAdapter::SetVelocityRigidbody(const RigidbodyId& rigidBodyId, const vec3& velocity)
{
    impl_->rigidbodies_.at(*rigidBodyId).velocity_ = velocity;
}

void BachPhysicsAdapter::ApplyImpulse(const RigidbodyId&, const vec3&)
{
}
void BachPhysicsAdapter::IncreaseVelocityRigidbody(const RigidbodyId&, const vec3&)
{
}
std::optional<vec3> BachPhysicsAdapter::GetVelocity(const RigidbodyId&)
{
    return {};
}
void BachPhysicsAdapter::SetAngularFactor(const RigidbodyId&, float)
{
}

void BachPhysicsAdapter::SetAngularFactor(const RigidbodyId&, const vec3&)
{
}

std::optional<vec3> BachPhysicsAdapter::GetAngularFactor(const RigidbodyId&)
{
    return std::nullopt;
}
void BachPhysicsAdapter::RemoveRigidBody(const RigidbodyId& id)
{
    impl_->rigidbodies_.erase(*id);
}
void BachPhysicsAdapter::RemoveShape(const ShapeId&)
{
}
void BachPhysicsAdapter::SetRotation(const RigidbodyId&, const vec3&)
{
}
void BachPhysicsAdapter::SetRotation(const RigidbodyId&, const Quaternion&)
{
}
void BachPhysicsAdapter::SetPosition(const RigidbodyId&, const vec3&)
{
}
void BachPhysicsAdapter::SetRigidbodyScale(const RigidbodyId&, const vec3&)
{
}
void BachPhysicsAdapter::SetShapeScale(const ShapeId&, const vec3&)
{
}
std::optional<Quaternion> BachPhysicsAdapter::GetRotation(const RigidbodyId&) const
{
    return std::nullopt;
}

std::optional<common::Transform> BachPhysicsAdapter::GetTransfrom(const RigidbodyId&) const
{
    return std::nullopt;
}
std::optional<RayHit> BachPhysicsAdapter::RayTest(const vec3&, const vec3&) const
{
    return std::optional<RayHit>();
}
void BachPhysicsAdapter::enableVisualizatedRigidbody(const RigidbodyId&)
{
}
void BachPhysicsAdapter::disableVisualizatedRigidbody(const RigidbodyId&)
{
}
void BachPhysicsAdapter::enableVisualizationForAllRigidbodys()
{
}
void BachPhysicsAdapter::disableVisualizationForAllRigidbodys()
{
}

Physics::CollisionSubId BachPhysicsAdapter::setCollisionCallback(const RigidbodyId&,
                                                                 std::function<void(const CollisionContactInfo&)>)
{
    return {};
}

void BachPhysicsAdapter::celarCollisionCallback(const CollisionSubId&)
{
}
}  // namespace Physics
}  // namespace GameEngine
