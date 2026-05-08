#include "BoxShape.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/CollisionShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
const std::string BoxShape::name = {"BoxShape"};

REGISTER_COMPONENT(BoxShape)

BoxShape::BoxShape(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
    , size(1.f)
{
}
void BoxShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&BoxShape::InitShape, this));
}
void BoxShape::InitShape()
{
    collisionShapeId_ =
        componentContext_.physicsApi_.CreateBoxColider(positionOffset, thisObject_.GetWorldTransform().GetScale(), size / 2.f);
}
void BoxShape::read(const TreeNode& node)
{
    ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
    ::Read(node.getChild(CSTR_SIZE), size);
}
void BoxShape::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_POSITION_OFFSET), positionOffset);
    ::write(node.addChild(CSTR_SIZE), size);
}
BoxShape& BoxShape::SetSize(const vec3& v)
{
    size = v;
    return *this;
}
BoxShape& BoxShape::SetSize(float v)
{
    size = vec3(v);
    return *this;
}
}  // namespace Components
}  // namespace GameEngine
