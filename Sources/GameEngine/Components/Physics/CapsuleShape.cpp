#include "CapsuleShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
ComponentsType CapsuleShape::type = ComponentsType::CapsuleShape;

CapsuleShape::CapsuleShape(const ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(CapsuleShape::type, componentContext, gameObject)
{
}
void CapsuleShape::Update()
{
}
void CapsuleShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&CapsuleShape::OnAwake, this));
}
void CapsuleShape::OnAwake()
{
    collisionShapeId_ = componentContext_.physicsApi_.CreateCapsuleColider(positionOffset_, radius_, height_);
}
CapsuleShape& CapsuleShape::SetHeight(float h)
{
    height_ = h;
    return *this;
}
CapsuleShape& CapsuleShape::SetRadius(float r)
{
    radius_ = r;
    return *this;
}
}  // namespace Components
}  // namespace GameEngine
