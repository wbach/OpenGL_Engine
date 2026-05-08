#include "CapsuleShape.h"

#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string CSTR_HEIGHT = "height";
}  // namespace

const std::string CapsuleShape::name = "CapsuleShape";

REGISTER_COMPONENT(CapsuleShape)

CapsuleShape::CapsuleShape(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
    , height{1.f}
    , radius{0.5f}
{
}
void CapsuleShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&CapsuleShape::InitShape, this));
}
void CapsuleShape::InitShape()
{
    collisionShapeId_ = componentContext_.physicsApi_.CreateCapsuleColider(
        positionOffset, thisObject_.GetWorldTransform().GetScale(), radius, height);

    if (collisionShapeId_)
    {
        setScale(thisObject_.GetWorldTransform().GetScale());
    }
}
CapsuleShape& CapsuleShape::SetHeight(float h)
{
    height = h;
    return *this;
}
CapsuleShape& CapsuleShape::SetRadius(float r)
{
    radius = r;
    return *this;
}
void CapsuleShape::read(const TreeNode& node)
{
    ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
    ::Read(node.getChild(CSTR_HEIGHT), height);
    ::Read(node.getChild(CSTR_RADIUS), radius);
}
void CapsuleShape::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_HEIGHT), GetHeight());
    ::write(node.addChild(CSTR_RADIUS), GetRadius());
}
}  // namespace Components
}  // namespace GameEngine
