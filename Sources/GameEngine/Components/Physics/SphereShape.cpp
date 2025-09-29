#include "SphereShape.h"

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
std::string SphereShape::name = {"SphereShape"};

SphereShape::SphereShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(GetComponentType<SphereShape>(), componentContext, gameObject)
    , radius(1.f)
{
}
void SphereShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&SphereShape::InitShape, this));
}

void SphereShape::InitShape()
{
    if (collisionShapeId_)
    {
        LOG_WARN << "Shape already created. Id = " << collisionShapeId_;
        return;
    }

    collisionShapeId_ = componentContext_.physicsApi_.CreateSphereColider(
        positionOffset, thisObject_.GetWorldTransform().GetScale(), radius / 2.f);
    LOG_DEBUG << "Created id = " << collisionShapeId_;
}
SphereShape& SphereShape::SetRadius(float r)
{
    radius = r;
    return *this;
}
void SphereShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<SphereShape>(componentContext, gameObject);

        vec3 positionOffset(0.f);
        ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
        component->SetPostionOffset(positionOffset);

        float radius(1.f);
        ::Read(node.getChild(CSTR_RADIUS), radius);
        component->SetRadius(radius);
        return component;
    };
    regsiterComponentReadFunction(GetComponentType<SphereShape>(), readFunc);
}
void SphereShape::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, SphereShape::name});

    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_RADIUS), radius);
}
}  // namespace Components
}  // namespace GameEngine
