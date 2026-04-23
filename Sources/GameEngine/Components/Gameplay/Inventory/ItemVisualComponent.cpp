#include "ItemVisualComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>
#include <Utils/XML/XmlReader.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Scene/SceneDef.h"
#include "Logger/Log.h"
namespace GameEngine
{
namespace
{
const char CSTR_ICON_PATH[]   = "iconPath";
const char CSTR_MODEL[]       = "model";
const char CSTR_DROP_SOUND[]  = "dropSound";
const char CSTR_MODEL_SCALE[] = "modelScale";
const char CSTR_MODEL_PATH[]  = "modelPath";
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

        if (const auto& modelNode = input.getChild(CSTR_MODEL))
        {
            if (const auto& rendererComponentNode = modelNode->getChild(CSTR_COMPONENT))
            {
                component->rendererComponentNode = *rendererComponentNode;
            }
        }

        if (not component->modelPath.empty())
        {
            LOG_DEBUG << "Read renderer component file: " << component->modelPath;
            Utils::XmlReader reader;
            reader.Read(component->modelPath.GetAbsolutePath());
            if (auto root = reader.Get())
            {
                component->rendererComponentNode = *root;
                LOG_DEBUG <<  component->rendererComponentNode;
            }
        }

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<ItemVisualComponent>(), func);
}

void ItemVisualComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(CSTR_ICON_PATH), iconPath);

    if (rendererComponentNode and modelPath.empty())
    {
        auto& modelNode = node.addChild(CSTR_MODEL);
        modelNode.addChild(*rendererComponentNode);
    }
    else
    {
        ::write(node.addChild(CSTR_MODEL_PATH), modelPath);
    }

    ::write(node.addChild(CSTR_DROP_SOUND), dropSound);
    ::write(node.addChild(CSTR_MODEL_SCALE), modelScale);
}

const std::optional<TreeNode>& ItemVisualComponent::getRendererComponentNode() const
{
    return rendererComponentNode;
}
}  // namespace Components
}  // namespace GameEngine
