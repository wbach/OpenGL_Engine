#include "EquippableComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace
{
const char CSTR_SLOT[]    = "slot";
const char CSTR_STR_REQ[] = "strReq";
const char CSTR_DEX_REQ[] = "dexReq";
const char CSTR_INT_REQ[] = "intReq";
}  // namespace

namespace Components
{
REGISTER_COMPONENT(EquippableComponent)

EquippableComponent::EquippableComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
}

EquippableComponent::~EquippableComponent()
{
}

void EquippableComponent::CleanUp()
{
}
void EquippableComponent::Reload()
{
}
void EquippableComponent::ReqisterFunctions()
{
}

void EquippableComponent::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_SLOT), slot);
    ::write(node.addChild(CSTR_STR_REQ), strReq);
    ::write(node.addChild(CSTR_DEX_REQ), dexReq);
    ::write(node.addChild(CSTR_INT_REQ), intReq);
}

void EquippableComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_SLOT), slot);
    ::Read(input.getChild(CSTR_STR_REQ), strReq);
    ::Read(input.getChild(CSTR_DEX_REQ), dexReq);
    ::Read(input.getChild(CSTR_INT_REQ), intReq);
}
}  // namespace Components
}  // namespace GameEngine
