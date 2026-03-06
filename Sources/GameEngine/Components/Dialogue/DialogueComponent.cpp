#include "DialogueComponent.h"

#include <Utils/TreeNode.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace
{
constexpr char CSTR_DIALOG_FILE[]   = "dialogueFile";
constexpr char CSTR_START_NODE_ID[] = "startNodeID";
}  // namespace

Components::DialogueComponent::DialogueComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<DialogueComponent>(), componentContext, gameObject)
{
}
void Components::DialogueComponent::CleanUp()
{
}
void Components::DialogueComponent::Reload()
{
}
void Components::DialogueComponent::ReqisterFunctions()
{
}
void Components::DialogueComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<DialogueComponent>(componentContext, gameObject);
        component->read(node);
        ::Read(node.getChild(CSTR_DIALOG_FILE), component->dialogueFile);
        ::Read(node.getChild(CSTR_START_NODE_ID), component->startNodeID);
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<DialogueComponent>(), func);
}
void Components::DialogueComponent::write(TreeNode& node) const
{
    const std::string CSTR_TYPE = "type";
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(CSTR_DIALOG_FILE), dialogueFile);
    ::write(node.addChild(CSTR_START_NODE_ID), startNodeID);
}
}  // namespace GameEngine
