#include "Rigidbody.h"
#include "CollisionShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
ComponentsType Rigidbody::type = ComponentsType::Rigidbody;

Rigidbody::Rigidbody()
    : BaseComponent(ComponentsType::Rigidbody)
    , mass_(1.0f)
    , isStatic_(false)
    , collisionShape_(nullptr)
    , rigidBodyId(0)
{
}
Rigidbody::~Rigidbody()
{
    if (rigidBodyId == 0)
    {
        return;
    }

    physicsApi_->RemoveRigidBody(rigidBodyId);
    rigidBodyId = 0;
}
void Rigidbody::OnStart()
{
    if (collisionShape_ == nullptr)
    {
        return;
    }

    rigidBodyId = physicsApi_->CreateRigidbody(collisionShape_->GetCollisionShapeId(), thisObject->worldTransform,
                                               mass_, isStatic_);
}
void Rigidbody::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, std::bind(&Rigidbody::OnStart, this));
}
void Rigidbody::SetMass(float mass)
{
    mass_ = mass;
}
void Rigidbody::SetIsStatic(bool is)
{
    isStatic_ = is;
}
void Rigidbody::SetCollisionShape(CollisionShape* collisionShape)
{
    collisionShape_ = collisionShape;
}
void Rigidbody::SetVelocity(const vec3& velocity)
{
    physicsApi_->SetVelocityRigidbody(rigidBodyId, velocity);
}
}  // namespace Components
}  // namespace GameEngine