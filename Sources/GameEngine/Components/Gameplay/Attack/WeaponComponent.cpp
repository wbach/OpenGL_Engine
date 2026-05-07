#include "WeaponComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_SOCKETS[] = "socketOffsets";
}  // namespace
WeaponComponent::WeaponComponent(ComponentContext& componentContext, GameObject& gameObject)
    : ComponentCore(GetComponentType<WeaponComponent>(), componentContext, gameObject)
{
}
WeaponComponent::~WeaponComponent()
{
}
void WeaponComponent::CleanUp()
{
}
void WeaponComponent::Reload()
{
}
void WeaponComponent::ReqisterFunctions()
{
}
std::vector<vec3> WeaponComponent::GetWorldSocketPositions() const
{
    std::vector<vec3> worldPositions;
    worldPositions.reserve(socketOffsets.size());

    auto modelMatrix = thisObject_.GetWorldTransform().CalculateCurrentMatrix();

    for (const auto& offset : socketOffsets)
    {
        worldPositions.push_back(modelMatrix * vec4(offset, 1.f));
    }
    return worldPositions;
}
void WeaponComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<WeaponComponent>(componentContext, gameObject);
        component->read(input);
        ::Read(input.getChild(CSTR_SOCKETS), component->socketOffsets);
        ::Read(input.getChild(CSTR_RADIUS), component->radius);
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<WeaponComponent>(), func);
}
void WeaponComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    ComponentCore::write(node);
    ::write(node.addChild(CSTR_SOCKETS), socketOffsets);
    ::write(node.addChild(CSTR_RADIUS), radius);
}
}  // namespace Components
}  // namespace GameEngine
