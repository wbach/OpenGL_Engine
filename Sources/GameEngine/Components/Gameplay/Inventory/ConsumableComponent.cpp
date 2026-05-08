#include "ConsumableComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const char CSTR_EFFECT[]   = "effect";
const char CSTR_POWER[]    = "power";
const char CSTR_DURATION[] = "duration";
const char CSTR_CHARGES[]  = "charges";
}  // namespace

REGISTER_COMPONENT(ConsumableComponent)

ConsumableComponent::ConsumableComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
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

void ConsumableComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_EFFECT), effect);
    ::Read(input.getChild(CSTR_POWER), power);
    ::Read(input.getChild(CSTR_DURATION), duration);
    ::Read(input.getChild(CSTR_CHARGES), charges);
}

void ConsumableComponent::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_EFFECT), effect);
    ::write(node.addChild(CSTR_POWER), power);
    ::write(node.addChild(CSTR_DURATION), duration);
    ::write(node.addChild(CSTR_CHARGES), charges);
}

}  // namespace Components
}  // namespace GameEngine
