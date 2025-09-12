#include "GuiElementReader.h"

#include <Logger/Log.h>
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <Utils/XML/XmlReader.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>

#include "Button/GuiButton.h"
#include "EditText/GuiEditText.h"
#include "GuiElement.h"
#include "GuiElementFactory.h"
#include "GuiElementsDef.h"
#include "GuiManager.h"
#include "Layout/HorizontalLayout.h"
#include "Layout/VerticalLayout.h"
#include "Text/GuiTextElement.h"
#include "Texutre/GuiTextureElement.h"
#include "TreeView/TreeView.h"
#include "Window/GuiWindow.h"

namespace GameEngine
{
namespace
{
GuiTheme ReadTheme(TreeNode &node)
{
    GuiTheme theme;
    for (auto &child : node.getChildren())
    {
        if (child->name() == Gui::BACKGROUND_TEXTURE)
        {
            theme.backgroundTexture = child->value_;
        }
        else if (child->name() == Gui::BUTTON_TEXTURE)
        {
            theme.buttonTexture = child->value_;
        }
        else if (child->name() == "buttonHoverTexture")
        {
            theme.buttonHoverTexture = child->value_;
        }
        else if (child->name() == "buttonActiveTexture")
        {
            theme.buttonActiveTexture = child->value_;
        }
    }
    return theme;
}
Layout::Algin ReadLayoutAlgin(TreeNode &node)
{
    auto paramNode = node.getChild(Gui::ALGIN);
    if (paramNode)
    {
        if (paramNode->value_ == Gui::LEFT)
        {
            return Layout::Algin::LEFT;
        }
        else if (paramNode->value_ == Gui::CENTER)
        {
            return Layout::Algin::CENTER;
        }
        else if (paramNode->value_ == Gui::RIGHT)
        {
            return Layout::Algin::RIGHT;
        }
    }

    return Layout::Algin::LEFT;
}
}  // namespace
GuiElementReader::GuiElementReader(GuiManager &manager, GuiElementFactory &factory)
    : manager_(manager)
    , factory_(factory)
{
}

bool GuiElementReader::Read(const std::string &filename)
{
    /* LOG TO FIX*/  LOG_ERROR << (filename);

    if (not Utils::CheckExtension(filename, "xml"))
    {
        /* LOG TO FIX*/  LOG_ERROR << ("This is not xml file. Format should be \".xml\". File name : " + filename);
        return false;
    }

    auto fileContent = Utils::ReadFile(filename);

    if (fileContent.empty())
    {
        /* LOG TO FIX*/  LOG_ERROR << (filename + " is empty!");
        return false;
    }

    Utils::XmlReader reader;
    if (not reader.ReadXml(fileContent))
    {
        return false;
    }

    auto guiNode = reader.Get(Gui::ROOT);
    if (not guiNode)
    {
        /* LOG TO FIX*/  LOG_ERROR << ("This is not gui file.");
        return false;
    }

    auto theme = guiNode->getChild(Gui::THEME);

    auto oldTheme = factory_.GetTheme();
    if (theme)
    {
        factory_.SetTheme(ReadTheme(*theme));
    }

    auto children = ReadChildrenElemets(*guiNode);

    const std::string &layerName = filename;
    manager_.AddLayer(layerName);
    for (auto &child : children)
    {
        manager_.Add(layerName, std::move(child));
    }

    if (theme)
    {
        factory_.SetTheme(oldTheme);
    }
    return true;
}

void GuiElementReader::ReadGuiElementBasic(GuiElement &element, TreeNode &node)
{
    auto paramNode = node.getChild(Gui::POSITION);
    if (paramNode)
    {
        vec2 position(0);
        ::Read(*paramNode, position);
        element.SetLocalPostion(position);
    }
    paramNode = node.getChild(Gui::SHOW);
    if (paramNode)
    {
        bool isShow(true);
        ::Read(*paramNode, isShow);
        element.Show(isShow);
    }

    paramNode = node.getChild(Gui::SCALE);
    if (paramNode)
    {
        vec2 scale(1.f);
        ::Read(*paramNode, scale);
        element.SetLocalScale(scale);
    }

    paramNode = node.getChild(Gui::LABEL);
    if (paramNode)
    {
        element.SetLabel(paramNode->value_);
    }

    paramNode = node.getChild(Gui::STARTUP_FUNCTION);

    if (paramNode and not paramNode->value_.empty())
    {
        element.SetStartupFunctionName(paramNode->value_);
        auto action = manager_.GetActionFunction(paramNode->value_);

        if (action)
        {
            action(element);
        }
    }
}

std::unique_ptr<GuiTextElement> GuiElementReader::ReadGuiText(TreeNode &node)
{
    std::string font = "GUI/Ubuntu-M.ttf", value = "empty string";
    uint32 fontSize = 10, outline = 0;

    auto paramNode = node.getChild(Gui::FONT);
    if (paramNode)
    {
        font = paramNode->value_;
    }

    paramNode = node.getChild(Gui::VALUE);
    if (paramNode)
    {
        value = paramNode->value_;
    }

    paramNode = node.getChild(Gui::FONT_SIZE);
    if (paramNode)
    {
        try
        {
            fontSize = std::stoi(paramNode->value_);
        }
        catch (...)
        {
            /* LOG TO FIX*/  LOG_ERROR << ("Read gui file, parse font size error.");
        }
    }

    paramNode = node.getChild(Gui::FONT_OUTLINE);
    if (paramNode)
    {
        try
        {
            outline = std::stoi(paramNode->value_);
        }
        catch (...)
        {
            /* LOG TO FIX*/  LOG_ERROR << ("Read gui file, parse outline error.");
        }
    }

    auto text = factory_.CreateGuiText(font, value, fontSize, outline);
    ReadGuiElementBasic(*text, node);

    paramNode = node.getChild(Gui::COLOR);
    if (paramNode)
    {
        vec4 color(0.f, 0.f, 0.f, 1.f);
        ::Read(*paramNode, color);

        /* LOG TO FIX*/  LOG_ERROR << ("SetColor " + std::to_string(color));
        text->SetColor(color);
    }
    return text;
}

std::unique_ptr<GuiTextureElement> GuiElementReader::ReadGuiTexture(TreeNode &node)
{
    std::string filename;
    auto paramNode = node.getChild(Gui::FILE);

    if (paramNode)
    {
        filename = paramNode->value_;
    }

    auto texture = factory_.CreateGuiTexture(filename);

    if (not texture)
    {
        return nullptr;
    }

    ReadGuiElementBasic(*texture, node);

    paramNode = node.getChild(Gui::COLOR);

    if (paramNode)
    {
        vec3 color(0);
        ::Read(*paramNode, color);
        texture->SetColor(color);
    }
    return texture;
}
std::unique_ptr<GuiButtonElement> GuiElementReader::ReadGuiButton(TreeNode &node)
{
    ActionFunction onClick;
    std::string actionName;
    auto paramNode = node.getChild(Gui::ACTION);
    if (paramNode)
    {
        actionName = paramNode->value_;
        onClick    = manager_.GetActionFunction(actionName);
    }
    auto button = factory_.CreateGuiButton(onClick);
    button->SetActionName(actionName);

    auto &children             = node.getChildren();
    auto backgroundTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->getChild(Gui::LABEL);
        if (c)
        {
            return c->value_ == Gui::BACKGROUND_TEXTURE;
        }
        return false;
    });

    if (backgroundTextureNode != children.end())
    {
        if ((**backgroundTextureNode).getChild(Gui::FILE)->value_ != Gui::NONE)
        {
            auto texture = ReadGuiTexture(**backgroundTextureNode);
            if (texture)
            {
                button->SetBackgroundTexture(std::move(texture));
            }
        }
        else
        {
            button->ResetBackgroundTexture();
        }
    }

    auto hoverTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->getChild(Gui::LABEL);
        if (c)
        {
            return c->value_ == Gui::HOVER_TEXTURE;
        }
        return false;
    });
    if (hoverTextureNode != children.end())
    {
        if ((**hoverTextureNode).getChild(Gui::FILE)->value_ != Gui::NONE)
        {
            auto texture = ReadGuiTexture(**hoverTextureNode);
            if (texture)
                button->SetOnHoverTexture(std::move(texture));
        }
        else
        {
            button->ResetOnHoverTexture();
        }
    }

    auto activeTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->getChild(Gui::LABEL);
        if (c)
        {
            return c->value_ == Gui::ACTIVE_TEXTURE;
        }
        return false;
    });

    if (activeTextureNode != children.end())
    {
        if ((**activeTextureNode).getChild(Gui::FILE)->value_ != Gui::NONE)
        {
            auto texture = ReadGuiTexture(**activeTextureNode);
            if (texture)
                button->SetOnActiveTexture(std::move(texture));
        }
        else
        {
            button->ResetOnActiveTexture();
        }
    }

    auto textNode = node.getChild(Gui::TEXT);
    if (textNode)
    {
        auto text = ReadGuiText(*textNode);
        if (text)
        {
            button->SetText(std::move(text));
        }
    }
    ReadGuiElementBasic(*button, node);
    return button;
}
std::unique_ptr<GuiEditBoxElement> GuiElementReader::ReadEditBox(TreeNode &node)
{
    auto textNode = node.getChild(Gui::TEXT);
    if (textNode)
    {
        auto text    = ReadGuiText(*textNode);
        auto editBox = factory_.CreateEditBox(std::move(text));
        ReadGuiElementBasic(*editBox, node);
        return editBox;
    }

    auto editBox = factory_.CreateEditBox();
    ReadGuiElementBasic(*editBox, node);
    return editBox;
}

VerticalLayout *GetVerticalLayoutById(VerticalLayout &root, uint32 id)
{
    if (root.GetId() == id and root.GetType() == GuiElementTypes::VerticalLayout)
        return &root;

    for (const auto &child : root.GetChildren())
    {
        if (child->GetType() != GuiElementTypes::VerticalLayout)
            continue;

        if (child->GetId() == id)
            return static_cast<VerticalLayout *>(child.get());

        auto result = GetVerticalLayoutById(*static_cast<VerticalLayout *>(child.get()), id);

        if (result)
            return result;
    }

    return nullptr;
}

std::unique_ptr<TreeView> GuiElementReader::ReadTreeView(TreeNode &treeNode)
{
    auto actionNode = treeNode.getChild(Gui::ACTION);
    auto action     = manager_.GetActionFunction(actionNode->value_);

    if (not action)
    {
        /* LOG TO FIX*/  LOG_ERROR << ("No action found for tree view \"" + actionNode->value_ + "\"");
        return nullptr;
    }

    auto treeView = factory_.CreateTreeView(action);
    ReadGuiElementBasic(*treeView, treeNode);

    auto elementsNode = treeNode.getChild(Gui::ELEMENTS);

    if (elementsNode)
    {
        for (const auto &node : elementsNode->getChildren())
        {
            auto x = node->getChild(Gui::X)->value_;
            auto y = node->getChild(Gui::Y)->value_;

            std::optional<uint32> parent{};
            if (y != "-")
            {
                parent = std::stoi(y);
            }

            treeView->Add(x, parent);
        }
    }

    return treeView;
}

std::vector<std::unique_ptr<GuiElement>> GuiElementReader::ReadChildrenElemets(TreeNode &node)
{
    std::vector<std::unique_ptr<GuiElement>> result;

    for (auto &child : node.getChildren())
    {
        if (child->name() == Gui::TEXT)
        {
            result.push_back(ReadGuiText(*child));
        }
        else if (child->name() == Gui::TEXTURE)
        {
            result.push_back(ReadGuiTexture(*child));
        }
        else if (child->name() == Gui::BUTTON)
        {
            result.push_back(ReadGuiButton(*child));
        }
        else if (child->name() == Gui::EDIT_BOX)
        {
            result.push_back(ReadEditBox(*child));
        }
        else if (child->name() == Gui::HORIZONTAL_LAYOUT)
        {
            result.push_back(ReadHorizontalLayout(*child));
        }
        else if (child->name() == Gui::VERTICAL_LAYOUT)
        {
            auto vlayout = ReadVerticalLayout(*child);
            if (vlayout)
            {
                result.push_back(std::move(vlayout));
            }
            else
            {
                /* LOG TO FIX*/  LOG_ERROR << ("read vertical layout error.");
            }
        }
        else if (child->name() == Gui::WINDOW)
        {
            auto window = ReadGuiWindow(*child);
            if (window)
            {
                result.push_back(std::move(window));
            }
            else
            {
                /* LOG TO FIX*/  LOG_ERROR << ("read window error.");
            }
        }
        else if (child->name() == Gui::TREE_VIEW)
        {
            auto treeView = ReadTreeView(*child);
            if (treeView)
            {
                result.push_back(std::move(treeView));
            }
            else
            {
                /* LOG TO FIX*/  LOG_ERROR << ("read tree view error.");
            }
        }
        else
        {
            /* LOG TO FIX*/  LOG_ERROR << ("try read unknown gui element type : " + child->name());
            continue;
        }
    }
    return result;
}

std::unique_ptr<VerticalLayout> GuiElementReader::ReadVerticalLayout(TreeNode &node)
{
    auto layout = factory_.CreateVerticalLayout();
    ReadGuiElementBasic(*layout, node);
    layout->SetAlgin(ReadLayoutAlgin(node));

    auto children = ReadChildrenElemets(node);
    for (auto &subChild : children)
    {
        layout->AddChild(std::move(subChild));
    }
    return layout;
}

std::unique_ptr<HorizontalLayout> GuiElementReader::ReadHorizontalLayout(TreeNode &node)
{
    auto layout = factory_.CreateHorizontalLayout();
    ReadGuiElementBasic(*layout, node);
    layout->SetAlgin(ReadLayoutAlgin(node));

    auto children = ReadChildrenElemets(node);
    for (auto &subChild : children)
    {
        layout->AddChild(std::move(subChild));
    }
    return layout;
}

std::unique_ptr<GuiWindowElement> GuiElementReader::ReadGuiWindow(TreeNode &node)
{
    std::string background;
    auto paramNode = node.getChild(Gui::BACKGROUND);
    if (paramNode)
    {
        background = paramNode->value_;
    }


    vec2 position(0.f), scale(0.25f);
    std::string style;

    ::Read(node.getChild(Gui::SCALE), scale);
    ::Read(node.getChild(Gui::POSITION), position);
    ::Read(node.getChild(Gui::STYLE), style);

    auto window = factory_.CreateGuiWindow(convert(style), position, scale, background);
    ReadGuiElementBasic(*window, node);

    auto children = ReadChildrenElemets(node);
    for (auto &subChild : children)
    {
        window->AddChild(std::move(subChild));
    }

    return window;
}

}  // namespace GameEngine
