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

    componentContext_.physicsApi_.RemoveRigidBody(rigidBodyId);
    rigidBodyId = 0;
}
void Rigidbody::OnStart()
{
    if (collisionShape_ == nullptr)
    {
        return;
    }

    rigidBodyId = componentContext_.physicsApi_.CreateRigidbody(collisionShape_->GetCollisionShapeId(), thisObject_.worldTransform,
                                               mass_, isStatic_);
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
    componentContext_.physicsApi_.SetVelocityRigidbody(rigidBodyId, velocity);
    return *this;
}
}  // namespace Components
}  // namespace GameEngine