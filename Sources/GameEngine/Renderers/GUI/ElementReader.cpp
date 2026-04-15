#include "ElementReader.h"
// // clang-format off
// // Shoulbe before read from TreeNode
// #include "GameEngine/Resources/File.h"
// // clang-format on

#include <Logger/Log.h>
#include <Types.h>
#include <Utils.h>
#include <Utils/Json/JsonReader.h>
#include <Utils/TreeNodeReadFunctions.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <filesystem>
#include <magic_enum/magic_enum.hpp>
#include <memory>

#include "Button/Button.h"
#include "EditText/EditText.h"
#include "Element.h"
#include "ElementsDef.h"
#include "GameEngine/Renderers/GUI/VerticalAlign.h"
#include "GameEngine/Renderers/GUI/Window/WindowStyle.h"
#include "GameEngine/Resources/FileHandle.h"
#include "IElementFactory.h"
#include "Layout/HorizontalLayout.h"
#include "Layout/VerticalLayout.h"
#include "Manager.h"
#include "Sprite/Sprite.h"
#include "Text/MultiLineText.h"
#include "Text/Text.h"
#include "Theme.h"
#include "TreeNode.h"
#include "Window/Window.h"

namespace GameEngine
{
namespace GUI
{
namespace
{
class ThemeDeserializer
{
public:
    Theme Deserialize(const TreeNode &root)
    {
        Theme theme;

        Read(root.getChild("DefaultText"), theme.text);
        Read(root.getChild("DefaultSprite"), theme.sprite);
        Read(root.getChild("EditText"), theme.editText);

        if (auto *buttonNode = root.getChild("Button"))
        {
            Read(buttonNode->getChild("Normal"), theme.button.normal);
            Read(buttonNode->getChild("Hover"), theme.button.hover);
            Read(buttonNode->getChild("Active"), theme.button.active);
            Read(buttonNode->getChild("Disabled"), theme.button.disabled);
        }

        if (auto *windowNode = root.getChild("Window"))
        {
            Read(windowNode->getChild("WindowBar"), theme.window.windowBar);
            Read(windowNode->getChild("Background"), theme.window.background);
        }

        return theme;
    }

private:
    void Read(const TreeNode *node, Theme::Text &v)
    {
        if (!node)
            return;
        ::Read(node->getChild("Color"), v.color);
        ::Read(node->getChild("Outline"), v.fontOutline);
        ::Read(node->getChild("Size"), v.fontSize_);

        if (auto *fontNode = node->getChild("Font"))
        {
            ::Read(*fontNode, v.font);
        }
    }

    void Read(const TreeNode *node, Theme::Sprite &v)
    {
        if (!node)
            return;

        ::Read(node->getChild("Color"), v.color);

        if (auto *texNode = node->getChild("Texture"))
        {
            ::Read(*texNode, v.baseTexture);
        }
    }

    void Read(const TreeNode *node, Theme::Button::StateStyle &v)
    {
        if (!node)
            return;

        Read(node->getChild("Text"), v.text);
        Read(node->getChild("Sprite"), v.sprite);
    }

    void Read(const TreeNode *node, Theme::EditText &v)
    {
        if (!node)
            return;
        Read(node->getChild("Text"), v.text);
        Read(node->getChild("Background"), v.background);
    }
};

Theme ReadTheme(const TreeNode &node)
{
    return ThemeDeserializer().Deserialize(node);
}
HorizontalAlign readHorizonalAlign(const TreeNode &node)
{
    auto paramNode = node.getChild(HORIZONAL_ALIGN);
    if (paramNode)
    {
        if (auto maybeResult = magic_enum::enum_cast<HorizontalAlign>(paramNode->value_))
        {
            return *maybeResult;
        }
    }

    return HorizontalAlign::LEFT;
}
VerticalAlign readVerticalAlign(const TreeNode &node)
{
    auto paramNode = node.getChild(VERTICAL_ALIGN);
    if (paramNode)
    {
        if (auto maybeResult = magic_enum::enum_cast<VerticalAlign>(paramNode->value_))
        {
            return *maybeResult;
        }
    }

    return VerticalAlign::CENTER;
}
}  // namespace
ElementReader::ElementReader(Manager &manager, IElementFactory &factory)
    : manager_(manager)
    , factory_(factory)
{
}

bool ElementReader::read(const File &file, const std::string &layerName)
{
    LOG_DEBUG << file;

    if (not file.IsFormat("gui"))
    {
        LOG_ERROR << "This is not gui file. Format should be \".xml\". File name : " << file;
        return false;
    }

    Utils::JsonReader reader;
    if (not reader.Read(file.GetAbsolutePath()))
    {
        return false;
    }

    auto guiNode = reader.Get(ROOT);
    if (not guiNode)
    {
        LOG_ERROR << "This is not gui file.";
        return false;
    }

    auto theme = guiNode->getChild(THEME);

    auto oldTheme = factory_.getTheme();
    if (theme)
    {
        factory_.setTheme(ReadTheme(*theme));
    }

    auto children = readElemets(*guiNode);

    auto &layer = manager_.createLayer(layerName);
    for (auto &child : children)
    {
        layer.add(std::move(child));
    }

    if (theme)
    {
        factory_.setTheme(oldTheme);
    }
    return true;
}

void ElementReader::readGuiElementBasic(Element &element, const TreeNode &node)
{
    if (auto paramNode = node.getChild(POSITION))
    {
        vec2 position(0);
        ::Read(*paramNode, position);
        element.setLocalPosition(position);
    }
    if (auto paramNode = node.getChild(SCALE))
    {
        vec2 scale(1.f);
        ::Read(*paramNode, scale);
        element.setLocalScale(scale);
    }
    if (auto paramNode = node.getChild(ACTIVE))
    {
        bool isShow(true);
        ::Read(*paramNode, isShow);
        element.activate(isShow);
    }
    if (auto paramNode = node.getChild(LABEL))
    {
        element.setLabel(paramNode->value_);
    }
}

std::unique_ptr<Text> ElementReader::readText(const TreeNode &node)
{
    auto text = factory_.createText("");

    Color color;
    ::Read(node.getChild(FONT), text->font.file);
    ::Read(node.getChild(VALUE), text->text.text);
    ::Read(node.getChild(FONT_SIZE), text->font.size);
    ::Read(node.getChild(FONT_OUTLINE), text->font.outline);
    ::Read(node.getChild(COLOR), color);
    ::Read(node.getChild(RENDER_MODE), text->render.mode);
    ::Read(node.getChild(HORIZONAL_ALIGN), text->render.align);
    ::Read(node.getChild(WRAP_WIDTH), text->text.wrapWidth);

    readGuiElementBasic(*text, node);
    text->setColor(color);

    LOG_DEBUG << " readText done";

    return text;
}

std::unique_ptr<MultiLineText> ElementReader::readMultiLineText(const TreeNode &node)
{
    auto text = factory_.createMultiLineText("");

    Color color;
    ::Read(node.getChild(FONT), text->font.file);
    ::Read(node.getChild(VALUE), text->text.text);
    ::Read(node.getChild(FONT_SIZE), text->font.size);
    ::Read(node.getChild(FONT_OUTLINE), text->font.outline);
    ::Read(node.getChild(COLOR), color);
    ::Read(node.getChild(RENDER_MODE), text->render.mode);
    ::Read(node.getChild(HORIZONAL_ALIGN), text->render.align);
    ::Read(node.getChild(WRAP_WIDTH), text->text.wrapWidth);
    ::Read(node.getChild(LINE_HEIGHT), text->lineHeight);

    readGuiElementBasic(*text, node);
    text->setColor(color);

    LOG_DEBUG << " readText done";

    return text;
}

std::unique_ptr<Sprite> ElementReader::readSprite(const TreeNode &node)
{
    FileHandle file;
    ::Read(node.getChild(FILE), file);

    auto texture = factory_.createSprite(file);

    readGuiElementBasic(*texture, node);

    if (auto paramNode = node.getChild(COLOR))
    {
        Color color(0);
        ::Read(*paramNode, color);
        texture->setColor(color);
    }
    return texture;
}
std::unique_ptr<Button> ElementReader::readButton(const TreeNode &node)
{
    auto button = factory_.createButton();
    button->setBackground(nullptr);
    button->setOnHover(nullptr);
    button->setOnActive(nullptr);

    if (auto paramnode = node.getChild(BACKGROUND_TEXTURE))
    {
        auto fileNode = paramnode->getChild(FILE);
        if (fileNode and not fileNode->value_.empty())
        {
            if (auto texture = readSprite(*paramnode))
            {
                button->setBackground(std::move(texture));
            }
        }
    }
    if (auto paramnode = node.getChild(HOVER_TEXTURE))
    {
        auto fileNode = paramnode->getChild(FILE);
        if (fileNode and not fileNode->value_.empty())
        {
            if (auto texture = readSprite(*paramnode))
            {
                button->setOnHover(std::move(texture));
            }
        }
    }
    if (auto paramnode = node.getChild(ACTIVE_TEXTURE))
    {
        auto fileNode = paramnode->getChild(FILE);
        if (fileNode and not fileNode->value_.empty())
        {
            if (auto texture = readSprite(*paramnode))
            {
                button->setOnActive(std::move(texture));
            }
        }
    }
    if (auto textNode = node.getChild(TEXT))
    {
        if (auto text = readText(*textNode))
        {
            button->setText(std::move(text));
        }
    }
    if (auto paramNode = node.getChild(BUTTON_TEXT_COLOR))
    {
        Color color(0);
        ::Read(*paramNode, color);
        button->setBackground(color);
    }
    if (auto paramNode = node.getChild(HOVER_TEXT_COLOR))
    {
        Color color(0);
        ::Read(*paramNode, color);
        button->setOnHover(color);
    }
    if (auto paramNode = node.getChild(ACTIVE_TEXT_COLOR))
    {
        Color color(0);
        ::Read(*paramNode, color);
        button->setOnActive(color);
    }

    readGuiElementBasic(*button, node);
    return button;
}
std::unique_ptr<EditText> ElementReader::readEditText(const TreeNode &node)
{
    auto edit = factory_.createEditText();

    if (auto textNode = node.getChild(TEXT))
    {
        edit->setText(textNode->value_);
    }

    readGuiElementBasic(*edit, node);
    return edit;
}

std::unique_ptr<Element> ElementReader::read(const TreeNode &node)
{
    std::unique_ptr<Element> result;
    auto typenode = node.getChild(TYPE);
    LOG_DEBUG << *typenode;
    if (not typenode)
    {
        result = std::make_unique<Element>();
        readGuiElementBasic(*result, node);
    }
    if (typenode->value_ == TEXT)
    {
        result = readText(node);
    }
    if (typenode->value_ == MULTI_LINE_TEXT)
    {
        result = readMultiLineText(node);
    }
    else if (typenode->value_ == SPRITE)
    {
        result = readSprite(node);
    }
    else if (typenode->value_ == BUTTON)
    {
        result = readButton(node);
    }
    else if (typenode->value_ == EDIT_TEXT)
    {
        result = readEditText(node);
    }
    else if (typenode->value_ == HORIZONTAL_LAYOUT)
    {
        result = readHorizontalLayout(node);
    }
    else if (typenode->value_ == VERTICAL_LAYOUT)
    {
        auto vlayout = readVerticalLayout(node);
        if (vlayout)
        {
            result = std::move(vlayout);
        }
        else
        {
            LOG_ERROR << "read vertical layout error.";
        }
    }
    else if (typenode->value_ == WINDOW)
    {
        auto window = readWindow(node);
        if (window)
        {
            result = std::move(window);
        }
        else
        {
            LOG_ERROR << "read window error.";
        }
    }

    if (result)
    {
        if (const auto childrenNode = node.getChild(CHILDREN))
        {
            for (auto &childNode : childrenNode->getChildren())
            {
                result->addChild(read(*childNode));
            }
        }
    }

    return result;
}

std::vector<std::unique_ptr<Element>> ElementReader::readElemets(const TreeNode &node)
{
    std::vector<std::unique_ptr<Element>> result;
    for (auto &elementNode : node.getChildren())
    {
        auto element = read(*elementNode);
        if (element)
        {
            result.push_back(std::move(element));
        }
    }
    return result;
}

std::unique_ptr<VerticalLayout> ElementReader::readVerticalLayout(const TreeNode &node)
{
    auto layout = factory_.createVerticalLayout();
    readGuiElementBasic(*layout, node);
    layout->setAlign(readHorizonalAlign(node));
    layout->setAlign(readVerticalAlign(node));

    if (auto n = node.getChild(AUTO_HIDE_ELEMENTS))
    {
        layout->autoHideElements(Utils::StringToBool(n->value_));
    }

    return layout;
}

std::unique_ptr<HorizontalLayout> ElementReader::readHorizontalLayout(const TreeNode &node)
{
    auto layout = factory_.createHorizontalLayout();
    readGuiElementBasic(*layout, node);
    layout->setAlign(readHorizonalAlign(node));
    layout->setAlign(readVerticalAlign(node));

    return layout;
}

std::unique_ptr<Window> ElementReader::readWindow(const TreeNode &node)
{
    std::string background;
    auto paramNode = node.getChild(BACKGROUND);
    if (paramNode)
    {
        background = paramNode->value_;
    }

    vec2 position(0.f), scale(0.25f);
    std::string style;

    ::Read(node.getChild(SCALE), scale);
    ::Read(node.getChild(POSITION), position);
    ::Read(node.getChild(STYLE), style);

    auto windowStyle = magic_enum::enum_cast<WindowStyle>(style).value_or(WindowStyle::BACKGROUND_ONLY);
    auto window      = factory_.createWindow(windowStyle);
    readGuiElementBasic(*window, node);
    return window;
}
}  // namespace GUI
}  // namespace GameEngine
