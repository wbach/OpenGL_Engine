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

REGISTER_COMPONENT(WeaponComponent)

WeaponComponent::WeaponComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
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
void WeaponComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_SOCKETS), socketOffsets);
    ::Read(input.getChild(CSTR_RADIUS), radius);
}
void WeaponComponent::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_SOCKETS), socketOffsets);
    ::write(node.addChild(CSTR_RADIUS), radius);
}
}  // namespace Components
}  // namespace GameEngine
