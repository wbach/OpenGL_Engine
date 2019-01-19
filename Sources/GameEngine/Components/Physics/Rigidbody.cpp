#include "Rigidbody.h"
#include "CollisionShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

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
Rigidbody& Rigidbody::SetCollisionShape(CollisionShape* collisionShape)
{
    collisionShape_ = collisionShape;
    return *this;
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
}  // namespace Components
}  // namespace GameEngine