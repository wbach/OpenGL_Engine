#include "BoxShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
ComponentsType BoxShape::type = ComponentsType::BoxShape;

BoxShape::BoxShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(ComponentsType::BoxShape, componentContext, gameObject)
    , size_(1.f)
{
}

void BoxShape::CleanUp()
{

}
void BoxShape::Update()
{
}
void BoxShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&BoxShape::OnAwake, this));
}
void BoxShape::OnAwake()
{
    collisionShapeId_ = componentContext_.physicsApi_.CreateBoxColider(positionOffset_, size_ / 2.f);
}
BoxShape& BoxShape::SetSize(const vec3& size)
{
    size_ = size;
    return *this;
}
BoxShape& BoxShape::SetSize(float size)
{
    size_ = vec3(size);
    return *this;
}
}  // namespace Components
}  // namespace GameEngine
