#include "CapsuleShape.h"

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

CapsuleShape::CapsuleShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(typeid(CapsuleShape).hash_code(), componentContext, gameObject)
{
}
void CapsuleShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&CapsuleShape::OnAwake, this));
}
void CapsuleShape::OnAwake()
{
    collisionShapeId_ = componentContext_.physicsApi_.CreateCapsuleColider(
        positionOffset_, thisObject_.GetWorldTransform().GetScale(), radius_, height_);

    if (collisionShapeId_)
    {
        setScale(thisObject_.GetWorldTransform().GetScale());
    }
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
void CapsuleShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<CapsuleShape>(componentContext, gameObject);

        vec3 positionOffset(0.f);
        ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
        component->SetPostionOffset(positionOffset);

        float height(1.f), radius(0.5f);
        ::Read(node.getChild(CSTR_HEIGHT), height);
        ::Read(node.getChild(CSTR_RADIUS), radius);
        component->SetHeight(height);
        component->SetRadius(radius);
        return component;
    };

    regsiterComponentReadFunction(CapsuleShape::name, readFunc);
}
void CapsuleShape::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, CapsuleShape::name});

    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_HEIGHT), GetHeight());
    ::write(node.addChild(CSTR_RADIUS), GetRadius());
}
}  // namespace Components
}  // namespace GameEngine
