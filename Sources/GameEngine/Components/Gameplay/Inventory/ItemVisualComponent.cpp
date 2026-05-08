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
REGISTER_COMPONENT(ItemVisualComponent)

ItemVisualComponent::ItemVisualComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
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
void ItemVisualComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_ICON_PATH), iconPath);
    ::Read(input.getChild(CSTR_MODEL_PATH), modelPath);
    ::Read(input.getChild(CSTR_DROP_SOUND), dropSound);
    ::Read(input.getChild(CSTR_MODEL_SCALE), modelScale);

    if (const auto& modelNode = input.getChild(CSTR_MODEL))
    {
        if (const auto& rendererComponentNode = modelNode->getChild(CSTR_COMPONENT))
        {
            this->rendererComponentNode = *rendererComponentNode;
        }
    }

    if (not modelPath.empty())
    {
        LOG_DEBUG << "Read renderer component file: " << modelPath;
        Utils::XmlReader reader;
        reader.Read(modelPath.GetAbsolutePath());
        if (auto root = reader.Get())
        {
            rendererComponentNode = *root;
            LOG_DEBUG << rendererComponentNode;
        }
    }
}
void ItemVisualComponent::write(TreeNode& node) const
{
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
