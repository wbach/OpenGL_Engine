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
void write(Utils::XmlNode& node, const GuiElement& element);

void writeChildren(Utils::XmlNode& node, const GuiElement& element)
{
    for (const auto& child : element.GetChildren())
    {
        write(node, *child);
    }
}

void write(Utils::XmlNode& node, const vec4& v)
{
    node.attributes_.insert({ Gui::X, std::to_string(v.x) });
    node.attributes_.insert({ Gui::Y, std::to_string(v.y) });
    node.attributes_.insert({ Gui::Z, std::to_string(v.z) });
    node.attributes_.insert({ Gui::W, std::to_string(v.w) });
}

void write(Utils::XmlNode& node, const vec3& v)
{
    node.attributes_.insert({Gui::X, std::to_string(v.x)});
    node.attributes_.insert({Gui::Y, std::to_string(v.y)});
    node.attributes_.insert({Gui::Z, std::to_string(v.z)});
}

void write(Utils::XmlNode& node, const vec2& v)
{
    node.attributes_.insert({Gui::X, std::to_string(v.x)});
    node.attributes_.insert({Gui::Y, std::to_string(v.y)});
}

void write(Utils::XmlNode& node, const vec2ui& v)
{
    node.attributes_.insert({Gui::X, std::to_string(v.x)});
    node.attributes_.insert({Gui::Y, std::to_string(v.y)});
}

void write(Utils::XmlNode& node, bool v)
{
    node.value_ = Utils::BoolToString(v);
}

void write(Utils::XmlNode& node, std::optional<uint32> v)
{
    if (v)
        node.value_ = std::to_string(*v);
    else
        node.value_ = "-";
}

template <class T>
void write(Utils::XmlNode& node, T v)
{
    node.value_ = std::to_string(v);
}

void write(Utils::XmlNode& node, const std::string& v)
{
    node.value_ = v;
}

void writeBasicParams(Utils::XmlNode& node, const GuiElement& element)
{
    auto& position = node.AddChild(Gui::POSITION);
    write(position, element.GetPosition());
    auto& show = node.AddChild(Gui::SHOW);
    write(show, element.IsShow());
    auto& scale = node.AddChild(Gui::SCALE);
    write(scale, element.GetScale());
    auto& label = node.AddChild(Gui::LABEL);
    write(label, element.GetLabel());
    auto& startupFunctionName = node.AddChild(Gui::STARTUP_FUNCTION);
    write(startupFunctionName, element.GetStartupFunctionName());
}

Utils::XmlNode& write(Utils::XmlNode& node, const GuiTextElement& text)
{
    auto& textNode = node.AddChild(Gui::TEXT);

    writeBasicParams(textNode, text);
    auto& font = textNode.AddChild(Gui::FONT);
    write(font, EngineConf_RemoveDataPath(text.GetFontInfo().font_));
    auto& fontSize = textNode.AddChild(Gui::FONT_SIZE);
    write(fontSize, text.GetFontInfo().fontSize_);
    auto& outline = textNode.AddChild(Gui::FONT_OUTLINE);
    write(outline, text.GetFontInfo().outline_);
    auto& value = textNode.AddChild(Gui::VALUE);
    write(value, text.GetText());
    auto& color = textNode.AddChild(Gui::COLOR);
    write(color, text.GetColor());
    return textNode;
}

Utils::XmlNode& write(Utils::XmlNode& node, const GuiTextureElement& element)
{
    auto& textureNode = node.AddChild(Gui::TEXTURE);

    writeBasicParams(textureNode, element);
    auto& file = textureNode.AddChild(Gui::FILE);
    auto texture = element.GetTexture();
    write(file, texture ? texture->GetFile()->GetDataRelativeDir() : "");
    auto& color = textureNode.AddChild(Gui::COLOR);
    write(color, element.GetColor());
    return textureNode;
}

Utils::XmlNode& writeNoneTexture(Utils::XmlNode& node, const std::string& label)
{
    auto& textureNode = node.AddChild(Gui::TEXTURE);

    textureNode.AddChild(Gui::LABEL).value_ = label;
    textureNode.AddChild(Gui::FILE).value_  = Gui::NONE;
    return textureNode;
}

void write(Utils::XmlNode& node, const GuiButtonElement& button)
{
    auto& buttonNode = node.AddChild(Gui::BUTTON);

    writeBasicParams(buttonNode, button);
    auto& value = buttonNode.AddChild(Gui::ACTION);
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

void write(Utils::XmlNode& node, const GuiWindowElement& window)
{
    auto& windowNode = node.AddChild(Gui::WINDOW);
    writeBasicParams(windowNode, window);

    auto& styleNode  = windowNode.AddChild(Gui::STYLE);
    styleNode.value_ = convert(window.GetStyle());

    writeChildren(windowNode, window);
}

void write(Utils::XmlNode& node, const GuiEditBoxElement& editBox)
{
    auto& editBoxNode = node.AddChild(Gui::EDIT_BOX);
    writeBasicParams(editBoxNode, editBox);
    writeChildren(editBoxNode, editBox);
}

void write(Utils::XmlNode& node, Layout::Algin algin)
{
    auto& verticalLayutNode = node.AddChild(Gui::ALGIN);

    if (algin == Layout::Algin::LEFT)
        verticalLayutNode.value_ = Gui::LEFT;
    if (algin == Layout::Algin::RIGHT)
        verticalLayutNode.value_ = Gui::RIGHT;
    if (algin == Layout::Algin::CENTER)
        verticalLayutNode.value_ = Gui::CENTER;
}

void write(Utils::XmlNode& node, const VerticalLayout& verticalLayout)
{
    auto& verticalLayutNode = node.AddChild(Gui::VERTICAL_LAYOUT);
    writeBasicParams(verticalLayutNode, verticalLayout);
    write(verticalLayutNode, verticalLayout.GetAlgin());
    writeChildren(verticalLayutNode, verticalLayout);
}

void write(Utils::XmlNode& node, const HorizontalLayout& horizontalLayout)
{
    auto& horizontalLayoutNode = node.AddChild(Gui::HORIZONTAL_LAYOUT);
    writeBasicParams(horizontalLayoutNode, horizontalLayout);
    write(horizontalLayoutNode, horizontalLayout.GetAlgin());
    writeChildren(horizontalLayoutNode, horizontalLayout);
}

void write(Utils::XmlNode& node, const TreeView& treeView)
{
    auto& treeViewNode = node.AddChild(Gui::TREE_VIEW);
    writeBasicParams(treeViewNode, treeView);
    auto& value = treeViewNode.AddChild(Gui::ACTION);
    write(value, treeView.GetActionName());

    auto& elementsNode = treeViewNode.AddChild(Gui::ELEMENTS);
    for (const auto& element : treeView.GetAddedElementCommands())
    {
        auto& pairNode = elementsNode.AddChild(Gui::PAIR);
        auto& x        = pairNode.AddChild(Gui::X);
        auto& y        = pairNode.AddChild(Gui::Y);

        pairNode.attributes_.insert({Gui::ID, std::to_string(std::get<0>(element))});

        write(x, std::get<1>(element));
        write(y, std::get<2>(element));
    }
}

void write(Utils::XmlNode& node, const GuiElement& element)
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
    Utils::XmlNode root(Gui::ROOT);

    for (const auto& element : layer.GetElements())
    {
        write(root, *element);
    }

    Utils::Xml::Write(name, root);
}
}  // namespace GuiElementWriter
}  // namespace GameEngine
