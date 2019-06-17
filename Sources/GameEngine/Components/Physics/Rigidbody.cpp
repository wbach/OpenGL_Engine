
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
    , mass_(1.0f)
    , isStatic_(false)
    , isInitilized_(false)
    , collisionShape_(nullptr)
    , rigidBodyId_(0)
    , velocity_(0)
    , shapeType_(ComponentsType::Rigidbody)
{
}
Rigidbody::~Rigidbody()
{
    if (rigidBodyId_ == 0)
    {
        return;
    }

    componentContext_.physicsApi_.RemoveRigidBody(rigidBodyId_);
    rigidBodyId_ = 0;
}
void Rigidbody::OnStart()
{
    GetCollisionShape();

    if (collisionShape_ == nullptr)
    {
        return;
    }

    rigidBodyId_ = componentContext_.physicsApi_.CreateRigidbody(collisionShape_->GetCollisionShapeId(),
                                                                 thisObject_.worldTransform, mass_, isStatic_);
    componentContext_.physicsApi_.SetVelocityRigidbody(rigidBodyId_, velocity_);
    isInitilized_ = true;
}
void Rigidbody::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, std::bind(&Rigidbody::OnStart, this));
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
    if (isInitilized_)
    {
        componentContext_.physicsApi_.SetVelocityRigidbody(rigidBodyId_, velocity);
    }
    else
    {
        velocity_ = velocity;
    }

    return *this;
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
const vec3& Rigidbody::GetVelocity() const
{
    return velocity_;
}
void Rigidbody::GetCollisionShape()
{
    switch (shapeType_)
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
            ERROR_LOG("Shape type (" + std::to_string(static_cast<int>(shapeType_)) + ") is not found.");
            break;
    };
}
}  // namespace Components
}  // namespace GameEngine
