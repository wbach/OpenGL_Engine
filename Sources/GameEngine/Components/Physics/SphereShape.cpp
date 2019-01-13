#include "SphereShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
ComponentsType SphereShape::type = ComponentsType::SphereShape;

SphereShape::SphereShape(const ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(ComponentsType::SphereShape, componentContext, gameObject)
    , size_(1.f)
{
}
void SphereShape::Update()
{
}
void SphereShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&SphereShape::OnAwake, this));
}
void SphereShape::OnAwake()
{
    collisionShapeId_ = componentContext_.physicsApi_.CreateSphereColider(positionOffset_, size_ / 2.f);
}
SphereShape& SphereShape::SetSize(float size)
{
    size_ = size;
    return *this;
}
}  // namespace Components
}  // namespace GameEngine
