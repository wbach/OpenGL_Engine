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
overloaded(Ts...)->overloaded<Ts...>;

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
    std::visit(overloaded{
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
    Rigidbody(common::Transform& transform, float mass, bool isStatic, uint32 shapeId)
        : transform_(transform)
        , mass_(mass)
        , isStatic_(isStatic)
        , shapeId_(shapeId)
    {
    }

    vec3 velocity_;
    common::Transform& transform_;
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
void BachPhysicsAdapter::Simulate()
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
                rigidbody.second.transform_.SetPosition(collisionData->position_);
            }
        }
    }
}
const GraphicsApi::LineMesh& BachPhysicsAdapter::DebugDraw()
{
    return lineMesh_;
}
void BachPhysicsAdapter::SetSimulationStep(float)
{
    impl_->enableSimulation_ = 1.f;
}
void BachPhysicsAdapter::EnableSimulation()
{
    impl_->enableSimulation_ = true;
}
void BachPhysicsAdapter::DisableSimulation()
{
    impl_->enableSimulation_ = false;
}
uint32 BachPhysicsAdapter::CreateBoxColider(const vec3&, const vec3&)
{
    return impl_->id_++;
}
uint32 BachPhysicsAdapter::CreateSphereColider(const vec3& positionOffset, float radius)
{
    impl_->shapes_.insert({impl_->id_, {positionOffset, radius}});
    return impl_->id_++;
}
uint32 BachPhysicsAdapter::CreateCapsuleColider(const vec3&, float, float)
{
    return uint32();
}
uint32 BachPhysicsAdapter::CreateTerrainColider(const vec3&, const HeightMap&, const vec3&)
{
    // impl_->terrains_.emplace_back(size, data, vec3(0), hightFactor);
    return impl_->id_++;
}
uint32 BachPhysicsAdapter::CreateMeshCollider(const vec3&, const std::vector<float>&, const IndicesVector&, float)
{
    return impl_->id_++;
}
uint32 BachPhysicsAdapter::CreateRigidbody(uint32 shapeId, common::Transform& transform, float mass, bool isStatic)
{
    impl_->rigidbodies_.insert({impl_->id_, Rigidbody(transform, mass, isStatic, shapeId)});
    return impl_->id_++;
}
void BachPhysicsAdapter::SetVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity)
{
    impl_->rigidbodies_.at(rigidBodyId).velocity_ = velocity;
}

void BachPhysicsAdapter::ApplyImpulse(uint32, const vec3&)
{
}
void BachPhysicsAdapter::IncreaseVelocityRigidbody(uint32, const vec3&)
{
}
std::optional<vec3> BachPhysicsAdapter::GetVelocity(uint32)
{
    return {};
}
void BachPhysicsAdapter::SetAngularFactor(uint32, float)
{
}

void BachPhysicsAdapter::SetAngularFactor(uint32, const vec3&)
{
}

std::optional<vec3> BachPhysicsAdapter::GetAngularFactor(uint32)
{
    return std::nullopt;
}
void BachPhysicsAdapter::RemoveRigidBody(uint32 id)
{
    impl_->rigidbodies_.erase(id);
}
void BachPhysicsAdapter::SetRotation(uint32, const vec3&)
{
}
void BachPhysicsAdapter::SetRotation(uint32, const Quaternion&)
{
}
void BachPhysicsAdapter::SetPosition(uint32, const vec3&)
{
}

std::optional<Quaternion> BachPhysicsAdapter::GetRotation(uint32 rigidBodyId) const
{
    return std::nullopt;
}

std::optional<common::Transform> BachPhysicsAdapter::GetTransfrom(uint32 rigidBodyId) const
{
    return std::nullopt;
}
std::optional<RayHit> BachPhysicsAdapter::RayTest(const vec3&, const vec3&) const
{
    return std::optional<RayHit>();
}
void BachPhysicsAdapter::setVisualizatedRigidbody(uint32)
{
}
void BachPhysicsAdapter::enableVisualizationForAllRigidbodys()
{
}
void BachPhysicsAdapter::disableVisualizationForAllRigidbodys()
{
}
}  // namespace Physics
}  // namespace GameEngine
