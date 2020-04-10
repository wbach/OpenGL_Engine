#include "BachPhysicsAdapter.h"
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

void collision(const SphehreShape& a, const SphehreShape& b)
{
}

void collision(const TerrainShape& a, const SphehreShape& b)
{
}

void collision(const MeshShape& a, const SphehreShape& b)
{
}

void collision(const BoxShape& a, const SphehreShape& b)
{
}

void collision(const MeshShape& a, const TerrainShape& b)
{
}

void collision(const BoxShape& a, const TerrainShape& b)
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
    TerrainShape(const vec2ui& size, std::vector<float>* heights, const vec2& terrainPosition, float heightFactor)
        : heightFactor_(heightFactor)
    {
    }
    std::optional<CollisionData> collision(Rigidbody& sphere)
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
void BachPhysicsAdapter::DebugDraw(const mat4& viewMatrix, const mat4& projectionMatrix)
{
}
void BachPhysicsAdapter::SetSimulationStep(float step)
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
uint32 BachPhysicsAdapter::CreateBoxColider(const vec3& positionOffset, const vec3& size)
{
    return impl_->id_++;
}
uint32 BachPhysicsAdapter::CreateSphereColider(const vec3& positionOffset, float radius)
{
    impl_->shapes_.insert({impl_->id_, {positionOffset, radius}});
    return impl_->id_++;
}
uint32 BachPhysicsAdapter::CreateCapsuleColider(const vec3& positionOffset, float radius, float height)
{
    return uint32();
}
uint32 BachPhysicsAdapter::CreateTerrainColider(const vec3& positionOffset, const vec2ui& size,
                                                const std::vector<float>& data, const vec3& scale)
{
    // impl_->terrains_.emplace_back(size, data, vec3(0), hightFactor);
    return impl_->id_++;
}
uint32 BachPhysicsAdapter::CreateMeshCollider(const vec3& positionOffset, const std::vector<float>& data,
                                              const IndicesVector& indicies, float scaleFactor)
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
void BachPhysicsAdapter::IncreaseVelocityRigidbody(uint32 rigidBodyId, const vec3& velocity)
{
}
std::optional<vec3> BachPhysicsAdapter::GetVelocity(uint32 rigidBodyId)
{
    return {};
}
void BachPhysicsAdapter::SetAngularFactor(uint32 rigidBodyId, float value)
{
}
void BachPhysicsAdapter::RemoveRigidBody(uint32 id)
{
    impl_->rigidbodies_.erase(id);
}
void BachPhysicsAdapter::SetRotation(uint32 rigidBodyId, const vec3&)
{
}
void BachPhysicsAdapter::SetRotation(uint32 rigidBodyId, const Quaternion&)
{
}
void BachPhysicsAdapter::SetPosition(uint32 rigidBodyId, const vec3&)
{
}
}  // namespace Physics
}  // namespace GameEngine
