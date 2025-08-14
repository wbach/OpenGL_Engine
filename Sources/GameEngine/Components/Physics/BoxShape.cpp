#include "BoxShape.h"

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
    : CollisionShape(typeid(BoxShape).hash_code(), componentContext, gameObject)
    , size(1.f)
{
}
void BoxShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&BoxShape::OnAwake, this));
}
void BoxShape::OnAwake()
{
    collisionShapeId_ = componentContext_.physicsApi_.CreateBoxColider(
        positionOffset, thisObject_.GetWorldTransform().GetScale(), size / 2.f);
}
void BoxShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<BoxShape>(componentContext, gameObject);

        vec3 positionOffset(0.f);
        ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
        component->SetPostionOffset(positionOffset);

        vec3 size(1.f);
        ::Read(node.getChild(CSTR_SIZE), size);
        component->SetSize(size);

        return component;
    };
    regsiterComponentReadFunction(BoxShape::name, readFunc);
}
void BoxShape::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, BoxShape::name});

    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_SIZE), GetSize());
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
