#include "ItemVisualComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace
{
const char CSTR_ICON_PATH[]   = "iconPath";
const char CSTR_MODEL_PATH[]  = "modelPath";
const char CSTR_DROP_SOUND[]  = "dropSound";
const char CSTR_MODEL_SCALE[] = "modelScale";
}  // namespace

namespace Components
{
ItemVisualComponent::ItemVisualComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<ItemVisualComponent>(), componentContext, gameObject)
{
}

ItemVisualComponent::~ItemVisualComponent()
{
}

void ItemVisualComponent::CleanUp()
{
}
void ItemVisualComponent::Reload()
{
}
void ItemVisualComponent::ReqisterFunctions()
{
}

void ItemVisualComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<ItemVisualComponent>(componentContext, gameObject);
        component->read(input);

        ::Read(input.getChild(CSTR_ICON_PATH), component->iconPath);
        ::Read(input.getChild(CSTR_MODEL_PATH), component->modelPath);
        ::Read(input.getChild(CSTR_DROP_SOUND), component->dropSound);
        ::Read(input.getChild(CSTR_MODEL_SCALE), component->modelScale);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<ItemVisualComponent>(), func);
}

void ItemVisualComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(CSTR_ICON_PATH), iconPath);
    ::write(node.addChild(CSTR_MODEL_PATH), modelPath);
    ::write(node.addChild(CSTR_DROP_SOUND), dropSound);
    ::write(node.addChild(CSTR_MODEL_SCALE), modelScale);
}

}  // namespace Components
}  // namespace GameEngine
