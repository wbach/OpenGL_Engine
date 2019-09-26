#include "GuiElementWriter.h"
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/EditText/GuiEditText.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <Utils/Utils.h>
#include <Utils/XML/XmlWriter.h>
#include "GuiElementsDef.h"

namespace GameEngine
{
namespace GuiElementWriter
{
void write(Utils::XmlNode& node, const GuiElement& element);

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
void write(Utils::XmlNode& node, bool v)
{
    node.value_ = Utils::BoolToString(v);
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
    auto& color = node.AddChild(Gui::COLOR);
    write(color, element.GetColor());
    auto& inBackground = node.AddChild(Gui::INBACKGROUND);
    write(inBackground, element.IsBackground());
    auto& position = node.AddChild(Gui::POSITION);
    write(position, element.GetPosition());
    auto& show = node.AddChild(Gui::SHOW);
    write(show, element.IsShow());
    auto& scale = node.AddChild(Gui::SCALE);
    write(scale, element.GetScale());
    auto& label = node.AddChild(Gui::LABEL);
    write(label, element.GetLabel());
    auto& permamanet = node.AddChild(Gui::PERMAMENT);
    write(permamanet, element.IsPermament());
}
void write(Utils::XmlNode& node, const GuiTextElement& text)
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
}
void write(Utils::XmlNode& node, const GuiTextureElement& texture)
{
    auto& textureNode = node.AddChild(Gui::TEXTURE);

    writeBasicParams(textureNode, texture);
    auto& file = textureNode.AddChild(Gui::FILE);
    write(file, EngineConf_RemoveDataPath(texture.GetFilename()));
}
void writeNoneTexture(Utils::XmlNode& node, const std::string& label)
{
    auto& textureNode = node.AddChild(Gui::TEXTURE);

    textureNode.AddChild(Gui::LABEL).value_ = label;
    textureNode.AddChild(Gui::FILE).value_  = Gui::NONE;
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

    if (button.GetBackgroundTexture())
    {
        write(buttonNode, *button.GetBackgroundTexture());
    }
    else
    {
        writeNoneTexture(buttonNode, Gui::BACKGROUND_TEXTURE);
    }

    if (button.GetOnActiveTexture())
    {
        write(buttonNode, *button.GetOnActiveTexture());
    }
    else
    {
        writeNoneTexture(buttonNode, Gui::ACTIVE_TEXTURE);
    }

    if (button.GetOnHoverTexture())
    {
        write(buttonNode, *button.GetOnHoverTexture());
    }
    else
    {
        writeNoneTexture(buttonNode, Gui::HOVER_TEXTURE);
    }
}
void write(Utils::XmlNode& node, const GuiWindowElement& window)
{
    auto& windowNode = node.AddChild(Gui::WINDOW);

    writeBasicParams(windowNode, window);

    for (const auto& child : window.GetChildren())
    {
        write(windowNode, *child);
    }
}
void write(Utils::XmlNode& node, const GuiEditBoxElement& editBox)
{
    auto& editBoxNode = node.AddChild(Gui::EDIT_BOX);
    writeBasicParams(editBoxNode, editBox);

    if (editBox.GetText())
    {
        write(editBoxNode, *editBox.GetText());
    }
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

    for (const auto& child : verticalLayout.GetChildren())
    {
        write(verticalLayutNode, child->Get());
    }
}
void write(Utils::XmlNode& node, const HorizontalLayout& horizontalLayout)
{
    auto& horizontalLayoutNode = node.AddChild(Gui::HORIZONTAL_LAYOUT);
    writeBasicParams(horizontalLayoutNode, horizontalLayout);
    write(horizontalLayoutNode, horizontalLayout.GetAlgin());

    for (const auto& child : horizontalLayout.GetChildren())
    {
        write(horizontalLayoutNode, child->Get());
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
