#include "GuiElementWriter.h"

#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/EditText/GuiEditText.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/TreeView/TreeView.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <Logger/Log.h>
#include <Utils/Utils.h>
#include <Utils/XML/XmlWriter.h>

#include "GuiElementsDef.h"

namespace GameEngine
{
namespace GuiElementWriter
{
void write(TreeNode& node, const GuiElement& element);

void writeChildren(TreeNode& node, const GuiElement& element)
{
    for (const auto& child : element.GetChildren())
    {
        write(node, *child);
    }
}

void write(TreeNode& node, const File& v)
{
    if (not v.empty())
        node.value_ = v.GetDataRelativePath();
}

void writeBasicParams(TreeNode& node, const GuiElement& element)
{
    ::write(node.addChild(Gui::POSITION), element.GetLocalPosition());
    ::write(node.addChild(Gui::SHOW), element.IsShow());
    ::write(node.addChild(Gui::SCALE), element.GetLocalScale());
    ::write(node.addChild(Gui::LABEL), element.GetLabel());
    ::write(node.addChild(Gui::STARTUP_FUNCTION), element.GetStartupFunctionName());
}

TreeNode& write(TreeNode& node, const GuiTextElement& text)
{
    auto& textNode = node.addChild(Gui::TEXT);

    writeBasicParams(textNode, text);
    auto& font = textNode.addChild(Gui::FONT);
    write(font, text.GetFontInfo().file_);
    auto& fontSize = textNode.addChild(Gui::FONT_SIZE);
    write(fontSize, text.GetFontInfo().fontSize_);
    auto& outline = textNode.addChild(Gui::FONT_OUTLINE);
    write(outline, text.GetFontInfo().outline_);
    auto& value = textNode.addChild(Gui::VALUE);
    write(value, text.GetText());
    auto& color = textNode.addChild(Gui::COLOR);
    write(color, text.GetColor());
    return textNode;
}

TreeNode& write(TreeNode& node, const GuiTextureElement& element)
{
    auto& textureNode = node.addChild(Gui::TEXTURE);

    writeBasicParams(textureNode, element);
    auto& file   = textureNode.addChild(Gui::FILE);
    auto texture = element.GetTexture();
    write(file, texture ? texture->GetFile() : File());
    auto& color = textureNode.addChild(Gui::COLOR);
    write(color, element.GetColor());
    return textureNode;
}

TreeNode& writeNoneTexture(TreeNode& node, const std::string& label)
{
    auto& textureNode = node.addChild(Gui::TEXTURE);

    textureNode.addChild(Gui::LABEL).value_ = label;
    textureNode.addChild(Gui::FILE).value_  = Gui::NONE;
    return textureNode;
}

void write(TreeNode& node, const GuiButtonElement& button)
{
    auto& buttonNode = node.addChild(Gui::BUTTON);

    writeBasicParams(buttonNode, button);
    auto& value = buttonNode.addChild(Gui::ACTION);
    write(value, button.GetActionName());

    if (button.GetText())
    {
        write(buttonNode, *button.GetText());
    }

    button.GetBackgroundTexture() ? write(buttonNode, *button.GetBackgroundTexture())
                                  : writeNoneTexture(buttonNode, Gui::BACKGROUND_TEXTURE);
    button.GetOnActiveTexture() ? write(buttonNode, *button.GetOnActiveTexture())
                                : writeNoneTexture(buttonNode, Gui::ACTIVE_TEXTURE);
    button.GetOnHoverTexture() ? write(buttonNode, *button.GetOnHoverTexture())
                               : writeNoneTexture(buttonNode, Gui::HOVER_TEXTURE);
}

void write(TreeNode& node, const GuiWindowElement& window)
{
    auto& windowNode = node.addChild(Gui::WINDOW);
    writeBasicParams(windowNode, window);

    auto& styleNode  = windowNode.addChild(Gui::STYLE);
    styleNode.value_ = convert(window.GetStyle());

    writeChildren(windowNode, window);
}

void write(TreeNode& node, const GuiEditBoxElement& editBox)
{
    auto& editBoxNode = node.addChild(Gui::EDIT_BOX);
    writeBasicParams(editBoxNode, editBox);
    writeChildren(editBoxNode, editBox);
}

void write(TreeNode& node, Layout::Algin algin)
{
    auto& verticalLayutNode = node.addChild(Gui::ALGIN);

    if (algin == Layout::Algin::LEFT)
        verticalLayutNode.value_ = Gui::LEFT;
    if (algin == Layout::Algin::RIGHT)
        verticalLayutNode.value_ = Gui::RIGHT;
    if (algin == Layout::Algin::CENTER)
        verticalLayutNode.value_ = Gui::CENTER;
}

void write(TreeNode& node, const VerticalLayout& verticalLayout)
{
    auto& verticalLayutNode = node.addChild(Gui::VERTICAL_LAYOUT);
    writeBasicParams(verticalLayutNode, verticalLayout);
    write(verticalLayutNode, verticalLayout.GetAlgin());
    writeChildren(verticalLayutNode, verticalLayout);
}

void write(TreeNode& node, const HorizontalLayout& horizontalLayout)
{
    auto& horizontalLayoutNode = node.addChild(Gui::HORIZONTAL_LAYOUT);
    writeBasicParams(horizontalLayoutNode, horizontalLayout);
    write(horizontalLayoutNode, horizontalLayout.GetAlgin());
    writeChildren(horizontalLayoutNode, horizontalLayout);
}

void write(TreeNode& node, const TreeView& treeView)
{
    auto& treeViewNode = node.addChild(Gui::TREE_VIEW);
    writeBasicParams(treeViewNode, treeView);
    auto& value = treeViewNode.addChild(Gui::ACTION);
    write(value, treeView.GetActionName());

    auto& elementsNode = treeViewNode.addChild(Gui::ELEMENTS);
    for (const auto& element : treeView.GetAddedElementCommands())
    {
        auto& pairNode = elementsNode.addChild(Gui::PAIR);
        auto& x        = pairNode.addChild(Gui::X);
        auto& y        = pairNode.addChild(Gui::Y);

        pairNode.attributes_.insert({Gui::ID, std::to_string(std::get<0>(element))});

        write(x, std::get<1>(element));
        write(y, std::get<2>(element));
    }
}

void write(TreeNode& node, const GuiElement& element)
{
    switch (element.GetType())
    {
        case GuiElementTypes::Text:
            write(node, *static_cast<const GuiTextElement*>(&element));
            break;
        case GuiElementTypes::Button:
            write(node, *static_cast<const GuiButtonElement*>(&element));
            break;
        case GuiElementTypes::Window:
            write(node, *static_cast<const GuiWindowElement*>(&element));
            break;
        case GuiElementTypes::EditBox:
            write(node, *static_cast<const GuiEditBoxElement*>(&element));
            break;
        case GuiElementTypes::Texture:
            write(node, *static_cast<const GuiTextureElement*>(&element));
            break;
        case GuiElementTypes::Checkbox:
            DEBUG_LOG("Gui Checkbox write method not implemented.");
            break;
        case GuiElementTypes::ComboBox:
            DEBUG_LOG("Gui ComboBox write method not implemented.");
            break;
        case GuiElementTypes::TreeView:
            write(node, *static_cast<const TreeView*>(&element));
            break;
        case GuiElementTypes::VerticalLayout:
            write(node, *static_cast<const VerticalLayout*>(&element));
            break;
        case GuiElementTypes::HorizontalLayout:
            write(node, *static_cast<const HorizontalLayout*>(&element));
            break;
        case GuiElementTypes::Layer:
            break;
    }
}

void write(std::vector<GuiLayer> layers)
{
    for (const auto& layer : layers)
    {
        write(layer.GetName(), layer);
    }
}

void write(const std::string& name, const GuiLayer& layer)
{
    TreeNode root(Gui::ROOT);

    for (const auto& element : layer.GetElements())
    {
        write(root, *element);
    }

    Utils::Xml::Write(name, root);
}
}  // namespace GuiElementWriter
}  // namespace GameEngine
