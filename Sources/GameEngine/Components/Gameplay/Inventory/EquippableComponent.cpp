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
EquippableComponent::EquippableComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<EquippableComponent>(), componentContext, gameObject)
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

void EquippableComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<EquippableComponent>(componentContext, gameObject);
        component->read(input);

        ::Read(input.getChild(CSTR_SLOT), component->slot);
        ::Read(input.getChild(CSTR_STR_REQ), component->strReq);
        ::Read(input.getChild(CSTR_DEX_REQ), component->dexReq);
        ::Read(input.getChild(CSTR_INT_REQ), component->intReq);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<EquippableComponent>(), func);
}

void EquippableComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(CSTR_SLOT), slot);
    ::write(node.addChild(CSTR_STR_REQ), strReq);
    ::write(node.addChild(CSTR_DEX_REQ), dexReq);
    ::write(node.addChild(CSTR_INT_REQ), intReq);
}

}  // namespace Components
}  // namespace GameEngine
