#include "ElementWriter.h"

#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/Button.h>
#include <GameEngine/Renderers/GUI/EditText/EditText.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Sprite/Sprite.h>
#include <GameEngine/Renderers/GUI/Text/MultiLineText.h>
#include <GameEngine/Renderers/GUI/Text/Text.h>
#include <GameEngine/Renderers/GUI/Window/Window.h>
#include <Logger/Log.h>
#include <Utils/Json/JsonWriter.h>
#include <Utils/TreeNodeWriteFunctions.h>
#include <Utils/Utils.h>

#include "ElementsDef.h"
#include "GameEngine/Renderers/GUI/IElementVisitor.h"
#include "GameEngine/Renderers/GUI/Layer/Layer.h"

namespace GameEngine
{
namespace GUI
{
namespace ElementWriter
{
class ThemeSerializer
{
public:
    TreeNode Serialize(const Theme& theme)
    {
        TreeNode root("Theme");

        write(root.addChild("DefaultText"), theme.text);
        write(root.addChild("DefaultSprite"), theme.sprite);
        write(root.addChild("EditText"), theme.editText);

        auto& buttonNode = root.addChild("Button");
        write(buttonNode.addChild("Normal"), theme.button.normal);
        write(buttonNode.addChild("Hover"), theme.button.hover);
        write(buttonNode.addChild("Active"), theme.button.active);
        write(buttonNode.addChild("Disabled"), theme.button.disabled);

        auto& windowNode = root.addChild("Window");
        write(windowNode.addChild("WindowBar"), theme.window.windowBar);
        write(windowNode.addChild("Background"), theme.window.background);

        return root;
    }

private:
    void write(TreeNode& node, const Theme::Text& v)
    {
        ::write(node, "Color", v.color);
        ::write(node, "Outline", v.fontOutline);
        ::write(node, "Font", v.font);
        ::write(node, "Size", v.fontSize_);
    }

    void write(TreeNode& node, const Theme::Sprite& v)
    {
        ::write(node, "Color", v.color);
        if (v.baseTexture)
        {
            ::write(node, "Texture", v.baseTexture);
        }
    }

    void write(TreeNode& node, const Theme::Button::StateStyle& v)
    {
        write(node.addChild("Text"), v.text);
        write(node.addChild("Sprite"), v.sprite);
    }

    void write(TreeNode& node, const Theme::EditText& v)
    {
        write(node.addChild("Text"), v.text);
        write(node.addChild("Background"), v.background);
    }
};

void writeBasicParams(TreeNode& node, const Element& element)
{
    ::write(node.addChild(POSITION), element.getLocalPosition());
    ::write(node.addChild(ACTION), element.isActive());
    ::write(node.addChild(SCALE), element.getLocalScale());
    ::write(node.addChild(LABEL), element.getLabel());
}

void write(TreeNode& node, const Element& element)
{
    writeBasicParams(node, element);
}

void write(TreeNode& node, const MultiLineText& text)
{
    write(node, TYPE, MULTI_LINE_TEXT);
    writeBasicParams(node, text);
    write(node, FONT, text.font.file);
    write(node, FONT_SIZE, text.font.size);
    write(node, FONT_OUTLINE, text.font.outline);
    write(node, VALUE, text.getText());
    write(node, COLOR, text.getColor());
    write(node, WRAP_WIDTH, text.text.wrapWidth);
    write(node, RENDER_MODE, text.render.mode);
    write(node, HORIZONAL_ALIGN, text.render.align);
    write(node, LINE_HEIGHT, text.lineHeight);
}

void write(TreeNode& node, const Text& text)
{
    write(node, TYPE, TEXT);
    writeBasicParams(node, text);
    write(node, FONT, text.font.file);
    write(node, FONT_SIZE, text.font.size);
    write(node, FONT_OUTLINE, text.font.outline);
    write(node, VALUE, text.getText());
    write(node, COLOR, text.getColor());
    write(node, WRAP_WIDTH, text.text.wrapWidth);
    write(node, RENDER_MODE, text.render.mode);
    write(node, HORIZONAL_ALIGN, text.render.align);
}

void write(TreeNode& node, const Sprite& element)
{
    write(node.addChild(TYPE), SPRITE);
    writeBasicParams(node, element);

    auto texture = element.getTexture();
    write(node.addChild(FILE), texture ? texture->GetFile() : File());
    write(node.addChild(COLOR), element.getColor());
}

TreeNode& writeNoneTexture(TreeNode& node, const std::string& label)
{
    write(node.addChild(TYPE), BUTTON);
    node.addChild(LABEL).value_ = label;
    node.addChild(FILE).value_  = NONE;
    return node;
}

void write(TreeNode& node, const Button& button)
{
    write(node.addChild(TYPE), BUTTON);

    writeBasicParams(node, button);

    if (button.getTextElement())
    {
        write(node.addChild(TEXT), *button.getTextElement());
    }
    if (button.getBackgroundSprite())
    {
        write(node.addChild(BACKGROUND_TEXTURE), *button.getBackgroundSprite());
    }
    if (button.getOnActiveSpirte())
    {
        write(node.addChild(ACTIVE_TEXTURE), *button.getOnActiveSpirte());
    }
    if (button.getOnHoverSprite())
    {
        write(node.addChild(HOVER_TEXTURE), *button.getOnHoverSprite());
    }

    write(node.addChild(BUTTON_TEXT_COLOR), button.getBackgroundColor());
    write(node.addChild(HOVER_TEXT_COLOR), button.getOnHoverColor());
    write(node.addChild(ACTIVE_TEXT_COLOR), button.getActiveColor());
}

void write(TreeNode& node, const Window& window)
{
    write(node.addChild(TYPE), WINDOW);
    writeBasicParams(node, window);
    auto& styleNode  = node.addChild(STYLE);
    styleNode.value_ = magic_enum::enum_name(window.getStyle());
}

void write(TreeNode& node, const EditText& editBox)
{
    write(node.addChild(TYPE), EDIT_TEXT);
    writeBasicParams(node, editBox);
}

void write(TreeNode& node, const VerticalLayout& verticalLayout)
{
    write(node.addChild(TYPE), VERTICAL_LAYOUT);
    writeBasicParams(node, verticalLayout);
    write(node.addChild(VERTICAL_ALIGN), verticalLayout.getVerticalAlign());
    write(node.addChild(HORIZONAL_ALIGN), verticalLayout.getHorizontalAlign());
    write(node.addChild(AUTO_HIDE_ELEMENTS), verticalLayout.autoHideElements());
    write(node, verticalLayout.autoHideElements());
}

void write(TreeNode& node, const HorizontalLayout& horizontalLayout)
{
    write(node.addChild(TYPE), HORIZONTAL_LAYOUT);
    writeBasicParams(node, horizontalLayout);
    write(node.addChild(VERTICAL_ALIGN), horizontalLayout.getVerticalAlign());
    write(node.addChild(HORIZONAL_ALIGN), horizontalLayout.getHorizontalAlign());
}

class ElementWriterVisitor : public IElementVisitor
{
public:
    ElementWriterVisitor(TreeNode& node)
        : node(node)
    {
    }
    void visit(Element& element) override
    {
        write(node, element);
    }
    void visit(Text& element) override
    {
        write(node, element);
    }
    void visit(MultiLineText& element) override
    {
        write(node, element);
    }
    void visit(Sprite& element) override
    {
        write(node, element);
    }
    void visit(Button& element) override
    {
        write(node, element);
    }
    void visit(Window& element) override
    {
        write(node, element);
    }
    void visit(EditText& element) override
    {
        write(node, element);
    }
    void visit(VerticalLayout& element) override
    {
        write(node, element);
    }
    void visit(HorizontalLayout& element) override
    {
        write(node, element);
    }

private:
    TreeNode& node;
};

void write(TreeNode& node, Element& element)
{
    auto& elementNode = node.addChild(ELEMENT);
    ElementWriterVisitor v(elementNode);
    element.accept(v);

    if (not element.getChildren().empty())
    {
        auto& childrenNode = elementNode.addChild(CHILDREN);
        for (auto& child : element.getChildren())
        {
            write(childrenNode, *child);
        }
    }
}

void write(std::vector<Layer>& layers)
{
    for (const auto& layer : layers)
    {
        write(layer.getName(), layer);
    }
}

void write(const File& file, const Layer& layer)
{
    TreeNode root(ROOT);

    for (const auto& element : layer.get())
    {
        write(root, *element);
    }

    LOG_DEBUG << "Write to: " << file;
    Utils::Json::Write(file.GetAbsolutePath(), root);
}
void write(const File& file, const Theme& theme)
{
    auto root = ThemeSerializer().Serialize(theme);
    LOG_DEBUG << "Write to: " << file;
    Utils::Json::Write(file.GetAbsolutePath(), root);
}
}  // namespace ElementWriter
}  // namespace GUI
}  // namespace GameEngine
