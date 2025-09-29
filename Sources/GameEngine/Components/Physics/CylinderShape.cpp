#include "CylinderShape.h"

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

CylinderShape::CylinderShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(GetComponentType<CylinderShape>(), componentContext, gameObject)
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
void CylinderShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<CylinderShape>(componentContext, gameObject);

        vec3 positionOffset(0.f);
        ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
        component->SetPostionOffset(positionOffset);

        vec3 size(1.f);
        ::Read(node.getChild(CSTR_SIZE), size);
        component->SetSize(size);
        return component;
    };
    regsiterComponentReadFunction(GetComponentType<CylinderShape>(), readFunc);
}
void CylinderShape::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, CylinderShape::name});

    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_SIZE), size);
}
}  // namespace Components
}  // namespace GameEngine
