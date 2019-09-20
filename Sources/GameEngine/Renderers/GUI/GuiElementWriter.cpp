#include "GuiElementWriter.h"
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/EditText/GuiEditText.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <Utils/XML/XmlWriter.h>

namespace GameEngine
{
namespace GuiElementWriter
{
void write(Utils::XmlNode& node, const GuiElement& element);

void write(Utils::XmlNode& node, const vec3& v)
{
    node.attributes_.insert({"x", std::to_string(v.x)});
    node.attributes_.insert({"y", std::to_string(v.y)});
    node.attributes_.insert({"z", std::to_string(v.z)});
}
void write(Utils::XmlNode& node, const vec2& v)
{
    node.attributes_.insert({"x", std::to_string(v.x)});
    node.attributes_.insert({"y", std::to_string(v.y)});
}
void write(Utils::XmlNode& node, bool v)
{
    node.value_ = std::to_string(v);
}
void write(Utils::XmlNode& node, const std::string& v)
{
    node.value_ = v;
}
void writeBasicParams(Utils::XmlNode& node, const GuiElement& element)
{
    auto& color = node.AddChild("color");
    write(color, element.GetColor());
    auto& inBackground = node.AddChild("inBackground");
    write(inBackground, element.IsBackground());
    auto& position = node.AddChild("position");
    write(position, element.GetPosition());
    auto& show = node.AddChild("show");
    write(show, element.IsShow());
    auto& scale = node.AddChild("scale");
    write(scale, element.GetScale());
    auto& label = node.AddChild("label");
    write(label, element.GetLabel());
    auto& permamanet = node.AddChild("permamanet");
    write(permamanet, element.IsPermament());
}
void write(Utils::XmlNode& node, const GuiTextElement& text)
{
    auto& textNode = node.AddChild("text");
    writeBasicParams(textNode, text);
    auto& font = node.AddChild("font");
    write(font, text.GetFontInfo().font_);
    auto& fontSize = node.AddChild("fontSize");
    write(fontSize, text.GetFontInfo().fontSize_);
    auto& outline = node.AddChild("outline");
    write(outline, text.GetFontInfo().outline_);
    auto& value = node.AddChild("value");
    write(value, text.GetText());
}
void write(Utils::XmlNode& node, const GuiButtonElement& button)
{
    auto& buttonNode = node.AddChild("button");
    writeBasicParams(buttonNode, button);
}
void write(Utils::XmlNode& node, const GuiWindowElement& window)
{
    auto& windowNode = node.AddChild("window");
    writeBasicParams(windowNode, window);

    for (const auto& child : window.GetChildren())
    {
        write(windowNode, *child);
    }
}
void write(Utils::XmlNode& node, const GuiEditBoxElement& editBox)
{
    auto& editBoxNode = node.AddChild("editBox");
    writeBasicParams(editBoxNode, editBox);
}
void write(Utils::XmlNode& node, const GuiTextureElement& texture)
{
    auto& textureNode = node.AddChild("texture");
    writeBasicParams(textureNode, texture);
}
void write(Utils::XmlNode& node, const VerticalLayout& verticalLayut)
{
    auto& verticalLayutNode = node.AddChild("verticalLayout");
    writeBasicParams(verticalLayutNode, verticalLayut);

    for (const auto& child : verticalLayut.GetChildren())
    {
        write(verticalLayutNode, child->Get());
    }
}
void write(Utils::XmlNode& node, const HorizontalLayout& horizontalLayout)
{
    auto& horizontalLayoutNode = node.AddChild("horizontalLayout");
    writeBasicParams(horizontalLayoutNode, horizontalLayout);

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
    Utils::XmlNode root("gui");

    for (const auto& element : layer.GetElements())
    {
        write(root, *element);
    }

    Utils::Xml::Write(name, root);
}
}  // namespace GuiElementWriter
}  // namespace GameEngine
