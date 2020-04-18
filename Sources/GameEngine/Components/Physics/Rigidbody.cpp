
#include "Rigidbody.h"
#include "BoxShape.h"
#include "CapsuleShape.h"
#include "CollisionShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "Logger/Log.h"
#include "MeshShape.h"
#include "SphereShape.h"
#include "Terrain/TerrainShape.h"

namespace GameEngine
{
namespace Components
{
ComponentsType Rigidbody::type = ComponentsType::Rigidbody;

Rigidbody::Rigidbody(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Rigidbody, componentContext, gameObject)
    , collisionShape_(nullptr)
    , mass_(1.0f)
    , isStatic_(false)
{
}
Rigidbody::~Rigidbody()
{
    if (not rigidBodyId_)
    {
        return;
    }

    componentContext_.physicsApi_.RemoveRigidBody(*rigidBodyId_);
}
void Rigidbody::OnStart()
{
    GetCollisionShape();

    if (not collisionShape_)
    {
        ERROR_LOG("Can not create Rigidbody without shape.");
        return;
    }

    auto rigidBodyId = componentContext_.physicsApi_.CreateRigidbody(collisionShape_->GetCollisionShapeId(),
                                                                     thisObject_.worldTransform, mass_, isStatic_);

    if (rigidBodyId == 0)
        return;

    rigidBodyId_ = rigidBodyId;

    if (inputParams_.velocity_)
        componentContext_.physicsApi_.SetVelocityRigidbody(rigidBodyId, *inputParams_.velocity_);

    if (inputParams_.angularFactor_)
        componentContext_.physicsApi_.SetAngularFactor(rigidBodyId, *inputParams_.angularFactor_);
}
void Rigidbody::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, std::bind(&Rigidbody::OnStart, this));
}
bool Rigidbody::IsReady() const
{
    return rigidBodyId_.has_value();
}
Rigidbody& Rigidbody::SetMass(float mass)
{
    mass_ = mass;
    return *this;
}
Rigidbody& Rigidbody::SetIsStatic(bool is)
{
    isStatic_ = is;
    if (is)
    {
        SetMass(0.f);
    }
    return *this;
}
Rigidbody& Rigidbody::SetCollisionShape(ComponentsType shapeType)
{
    if (not isShapeTypeValid(shapeType))
    {
        ERROR_LOG("Shape type (" + std::to_string(static_cast<int>(shapeType)) + ") is not valid.");
        return *this;
    }

    shapeType_ = shapeType;
    return *this;
}
bool Rigidbody::isShapeTypeValid(ComponentsType shapeType)
{
    return shapeType == ComponentsType::BoxShape or shapeType == ComponentsType::TerrainShape or
           shapeType == ComponentsType::MeshShape or shapeType == ComponentsType::SphereShape or
           shapeType == ComponentsType::CapsuleShape;
}
Rigidbody& Rigidbody::SetVelocity(const vec3& velocity)
{
    componentContext_.physicsApi_.SetVelocityRigidbody(*rigidBodyId_, velocity);
    return *this;
}
Rigidbody& Rigidbody::SetAngularFactor(float v)
{
    componentContext_.physicsApi_.SetAngularFactor(*rigidBodyId_, v);
    return *this;
}

Rigidbody& Rigidbody::SetAngularFactor(const vec3& angularFactor)
{
    componentContext_.physicsApi_.SetAngularFactor(*rigidBodyId_, angularFactor);
    return *this;
}
Rigidbody& Rigidbody::SetRotation(const DegreesVec3& rotation)
{
    componentContext_.physicsApi_.SetRotation(*rigidBodyId_, rotation.Radians());
    return *this;
}
Rigidbody& Rigidbody::SetRotation(const RadiansVec3& rotation)
{
    componentContext_.physicsApi_.SetRotation(*rigidBodyId_, rotation.value);
    return *this;
}
Rigidbody& Rigidbody::SetRotation(const Quaternion& rotation)
{
    componentContext_.physicsApi_.SetRotation(*rigidBodyId_, rotation);
    return *this;
}
Rigidbody& Rigidbody::SetPosition(const vec3& pos)
{
    componentContext_.physicsApi_.SetPosition(*rigidBodyId_, pos);
    return *this;
}
void Rigidbody::ApplyImpulse(const vec3& v)
{
    componentContext_.physicsApi_.ApplyImpulse(*rigidBodyId_, v);
}
void Rigidbody::IncreaseVelocity(const vec3& v)
{
    componentContext_.physicsApi_.IncreaseVelocityRigidbody(*rigidBodyId_, v);
}
float Rigidbody::GetMass() const
{
    return mass_;
}
bool Rigidbody::IsStatic() const
{
    return isStatic_;
}
ComponentsType Rigidbody::GetCollisionShapeType() const
{
    return collisionShape_->GetType();
}
vec3 Rigidbody::GetVelocity() const
{
    return *componentContext_.physicsApi_.GetVelocity(*rigidBodyId_);
}
vec3 Rigidbody::GetAngularFactor() const
{
    return *componentContext_.physicsApi_.GetAngularFactor(*rigidBodyId_);
}

Quaternion Rigidbody::GetRotation() const
{
    return *componentContext_.physicsApi_.GetRotation(*rigidBodyId_);
}

common::Transform Rigidbody::GetTransform() const
{
    return *componentContext_.physicsApi_.GetTransfrom(*rigidBodyId_);
}

Rigidbody::Params& Rigidbody::InputParams()
{
    return inputParams_;
}

const Rigidbody::Params& Rigidbody::InputParams() const
{
    return inputParams_;
}
void Rigidbody::GetCollisionShape()
{
    if (not shapeType_)
        return;

    switch (*shapeType_)
    {
        case ComponentsType::BoxShape:
            collisionShape_ = thisObject_.GetComponent<BoxShape>();
            break;
        case ComponentsType::TerrainShape:
            collisionShape_ = thisObject_.GetComponent<TerrainShape>();
            break;
        case ComponentsType::MeshShape:
            collisionShape_ = thisObject_.GetComponent<MeshShape>();
            break;
        case ComponentsType::SphereShape:
            collisionShape_ = thisObject_.GetComponent<SphereShape>();
            break;
        case ComponentsType::CapsuleShape:
            collisionShape_ = thisObject_.GetComponent<CapsuleShape>();
            break;
        default:
            ERROR_LOG("Shape type (" + std::to_string(static_cast<int>(*shapeType_)) + ") is not found.");
            break;
    };
}
}  // namespace Components
}  // namespace GameEngine
