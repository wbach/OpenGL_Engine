#include "SphereShape.h"

#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
std::string SphereShape::name = {"SphereShape"};

REGISTER_COMPONENT(SphereShape)

SphereShape::SphereShape(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
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
void SphereShape::read(const TreeNode& node)
{
    ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
    ::Read(node.getChild(CSTR_RADIUS), radius);
}
void SphereShape::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_RADIUS), radius);
}
}  // namespace Components
}  // namespace GameEngine
