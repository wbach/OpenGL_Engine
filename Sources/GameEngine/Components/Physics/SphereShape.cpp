#include "SphereShape.h"

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
std::string SphereShape::name = { "SphereShape" };

SphereShape::SphereShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(typeid(SphereShape).hash_code(), componentContext, gameObject)
    , radius_(1.f)
{
}

void SphereShape::CleanUp()
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
    collisionShapeId_ = componentContext_.physicsApi_.CreateSphereColider(positionOffset_, radius_ / 2.f);
}
SphereShape& SphereShape::SetRadius(float radius)
{
    radius_ = radius;
    return *this;
}
void SphereShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<SphereShape>(componentContext, gameObject);

        vec3 positionOffset(0.f);
        ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
        component->SetPostionOffset(positionOffset);

        float radius(1.f);
        ::Read(node.getChild(CSTR_RADIUS), radius);
        component->SetRadius(radius);
        return component;
    };
    regsiterComponentReadFunction(SphereShape::name, readFunc);
}
void SphereShape::write(TreeNode& node) const
{
    node.attributes_.insert({ CSTR_TYPE, SphereShape::name });

    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_RADIUS), radius_);
}
}  // namespace Components
}  // namespace GameEngine
