#include "CylinderShape.h"

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
std::string CylinderShape::name = {"CylinderShape"};

REGISTER_COMPONENT(CylinderShape)

CylinderShape::CylinderShape(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
    , size(1.f)
{
}
void CylinderShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&CylinderShape::InitShape, this));
}
void CylinderShape::InitShape()
{
    collisionShapeId_ = componentContext_.physicsApi_.CreateCylinderColider(
        positionOffset, thisObject_.GetWorldTransform().GetScale(), size / 2.f);
}
CylinderShape& CylinderShape::SetSize(const vec3& v)
{
    size = v;
    return *this;
}
void CylinderShape::read(const TreeNode& node)
{
    ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
    ::Read(node.getChild(CSTR_SIZE), size);
}
void CylinderShape::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_SIZE), size);
}
const vec3& CylinderShape::GetSize() const
{
    return size;
}
}  // namespace Components
}  // namespace GameEngine
