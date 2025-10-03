#include "BoxShape.h"

#include <Utils/TreeNode.h>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
const std::string BoxShape::name = {"BoxShape"};

BoxShape::BoxShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(GetComponentType<BoxShape>(), componentContext, gameObject)
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
void BoxShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<BoxShape>(componentContext, gameObject);
        ::Read(node.getChild(CSTR_POSITION_OFFSET), component->positionOffset);
        ::Read(node.getChild(CSTR_SIZE), component->size);

        return component;
    };
    regsiterComponentReadFunction(GetComponentType<BoxShape>(), readFunc);
}
void BoxShape::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, BoxShape::name});

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
