#include "ConsumableComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace
{
const char CSTR_EFFECT[]   = "effect";
const char CSTR_POWER[]    = "power";
const char CSTR_DURATION[] = "duration";
const char CSTR_CHARGES[]  = "charges";
}  // namespace

namespace Components
{
ConsumableComponent::ConsumableComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<ConsumableComponent>(), componentContext, gameObject)
{
}

ConsumableComponent::~ConsumableComponent()
{
}

void ConsumableComponent::CleanUp()
{
}
void ConsumableComponent::Reload()
{
}
void ConsumableComponent::ReqisterFunctions()
{
}

void ConsumableComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<ConsumableComponent>(componentContext, gameObject);
        component->read(input);

        ::Read(input.getChild(CSTR_EFFECT), component->effect);
        ::Read(input.getChild(CSTR_POWER), component->power);
        ::Read(input.getChild(CSTR_DURATION), component->duration);
        ::Read(input.getChild(CSTR_CHARGES), component->charges);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<ConsumableComponent>(), func);
}

void ConsumableComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(CSTR_EFFECT), effect);
    ::write(node.addChild(CSTR_POWER), power);
    ::write(node.addChild(CSTR_DURATION), duration);
    ::write(node.addChild(CSTR_CHARGES), charges);
}

}  // namespace Components
}  // namespace GameEngine
