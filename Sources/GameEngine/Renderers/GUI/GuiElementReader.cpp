#include "GuiElementReader.h"

#include <Logger/Log.h>
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <Utils/XML/XmlReader.h>
#include <algorithm>

#include "GuiElement.h"
#include "GuiElementFactory.h"
#include "GuiElementsDef.h"
#include "GuiManager.h"

#include "Button/GuiButton.h"
#include "EditText/GuiEditText.h"
#include "Layout/HorizontalLayout.h"
#include "Layout/VerticalLayout.h"
#include "Text/GuiTextElement.h"
#include "Text/GuiTextFactory.h"
#include "Texutre/GuiTextureElement.h"
#include "TreeView/TreeView.h"
#include "Window/GuiWindow.h"

namespace GameEngine
{
namespace
{
GuiTheme ReadTheme(Utils::XmlNode &node)
{
    GuiTheme theme;
    for (auto &child : node.GetChildren())
    {
        if (child->GetName() == Gui::BACKGROUND_TEXTURE)
        {
            theme.backgroundTexture = child->value_;
        }
        else if (child->GetName() == Gui::BUTTON_TEXTURE)
        {
            theme.buttonTexture = child->value_;
        }
        else if (child->GetName() == "buttonHoverTexture")
        {
            theme.buttonHoverTexture = child->value_;
        }
        else if (child->GetName() == "buttonActiveTexture")
        {
            theme.buttonActiveTexture = child->value_;
        }
    }
    return theme;
}
Layout::Algin ReadLayoutAlgin(Utils::XmlNode &node)
{
    auto paramNode = node.GetChild(Gui::ALGIN);
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
    DEBUG_LOG(filename);

    if (not Utils::CheckExtension(filename, "xml"))
    {
        ERROR_LOG("This is not xml file. Format should be \".xml\". File name : " + filename);
        return false;
    }

    auto fileContent = Utils::ReadFile(filename);

    if (fileContent.empty())
    {
        ERROR_LOG(filename + " is empty!");
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
        ERROR_LOG("This is not gui file.");
        return false;
    }

    auto theme = guiNode->GetChild(Gui::THEME);

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

void GuiElementReader::ReadGuiElementBasic(GuiElement &element, Utils::XmlNode &node)
{
    auto paramNode = node.GetChild(Gui::POSITION);
    if (paramNode)
    {
        auto position = Utils::ReadVec2(*paramNode);
        element.SetPostion(position);
    }
    paramNode = node.GetChild(Gui::SHOW);
    if (paramNode)
    {
        element.Show(Utils::ReadBool(*paramNode));
    }

    paramNode = node.GetChild(Gui::SCALE);
    if (paramNode)
    {
        element.SetScale(Utils::ReadVec2(*paramNode));
    }

    paramNode = node.GetChild(Gui::LABEL);
    if (paramNode)
    {
        element.SetLabel(paramNode->value_);
    }

    paramNode = node.GetChild(Gui::STARTUP_FUNCTION);

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

std::unique_ptr<GuiTextElement> GuiElementReader::ReadGuiText(Utils::XmlNode &node)
{
    std::string font = "GUI/Ubuntu-M.ttf", value = "empty string";
    uint32 fontSize = 10, outline = 0;

    auto paramNode = node.GetChild(Gui::FONT);
    if (paramNode)
    {
        font = paramNode->value_;
    }

    paramNode = node.GetChild(Gui::VALUE);
    if (paramNode)
    {
        value = paramNode->value_;
    }

    paramNode = node.GetChild(Gui::FONT_SIZE);
    if (paramNode)
    {
        try
        {
            fontSize = std::stoi(paramNode->value_);
        }
        catch (...)
        {
            ERROR_LOG("Read gui file, parse font size error.");
        }
    }

    paramNode = node.GetChild(Gui::FONT_OUTLINE);
    if (paramNode)
    {
        try
        {
            outline = std::stoi(paramNode->value_);
        }
        catch (...)
        {
            ERROR_LOG("Read gui file, parse outline error.");
        }
    }

    auto text = factory_.CreateGuiText(font, value, fontSize, outline);
    ReadGuiElementBasic(*text, node);

    paramNode = node.GetChild(Gui::COLOR);
    if (paramNode)
    {
        auto color = Utils::ReadVec3(*paramNode);
        DEBUG_LOG("SetColor " + std::to_string(color));
        text->SetColor(color);
    }
    return text;
}

std::unique_ptr<GuiTextureElement> GuiElementReader::ReadGuiTexture(Utils::XmlNode &node)
{
    std::string filename;
    auto paramNode = node.GetChild(Gui::FILE);

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

    paramNode = node.GetChild(Gui::COLOR);
    if (paramNode)
        if (paramNode)
        {
            auto color = Utils::ReadVec3(*paramNode);
            texture->SetColor(color);
        }
    return texture;
}
std::unique_ptr<GuiButtonElement> GuiElementReader::ReadGuiButton(Utils::XmlNode &node)
{
    ActionFunction onClick;
    std::string actionName;
    auto paramNode = node.GetChild(Gui::ACTION);
    if (paramNode)
    {
        actionName = paramNode->value_;
        onClick    = manager_.GetActionFunction(actionName);
    }
    auto button = factory_.CreateGuiButton(onClick);
    button->SetActionName(actionName);

    auto &children             = node.GetChildren();
    auto backgroundTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->GetChild(Gui::LABEL);
        if (c)
        {
            return c->value_ == Gui::BACKGROUND_TEXTURE;
        }
        return false;
    });

    if (backgroundTextureNode != children.end())
    {
        if ((**backgroundTextureNode).GetChild(Gui::FILE)->value_ != Gui::NONE)
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
        auto c = child->GetChild(Gui::LABEL);
        if (c)
        {
            return c->value_ == Gui::HOVER_TEXTURE;
        }
        return false;
    });
    if (hoverTextureNode != children.end())
    {
        if ((**hoverTextureNode).GetChild(Gui::FILE)->value_ != Gui::NONE)
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
        auto c = child->GetChild(Gui::LABEL);
        if (c)
        {
            return c->value_ == Gui::ACTIVE_TEXTURE;
        }
        return false;
    });

    if (activeTextureNode != children.end())
    {
        if ((**activeTextureNode).GetChild(Gui::FILE)->value_ != Gui::NONE)
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

    auto textNode = node.GetChild(Gui::TEXT);
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
std::unique_ptr<GuiEditBoxElement> GuiElementReader::ReadEditBox(Utils::XmlNode &node)
{
    auto textNode = node.GetChild(Gui::TEXT);
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

std::unique_ptr<TreeView> GuiElementReader::ReadTreeView(Utils::XmlNode &treeNode)
{
    auto actionNode = treeNode.GetChild(Gui::ACTION);
    auto action     = manager_.GetActionFunction(actionNode->value_);

    if (not action)
    {
        DEBUG_LOG("No action found for tree view \"" + actionNode->value_ + "\"");
        return nullptr;
    }

    auto treeView = factory_.CreateTreeView(action);
    ReadGuiElementBasic(*treeView, treeNode);

    auto elementsNode = treeNode.GetChild(Gui::ELEMENTS);

    if (elementsNode)
    {
        for (const auto &node : elementsNode->GetChildren())
        {
            auto x = node->GetChild(Gui::X)->value_;
            auto y = node->GetChild(Gui::Y)->value_;

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

std::vector<std::unique_ptr<GuiElement>> GuiElementReader::ReadChildrenElemets(Utils::XmlNode &node)
{
    std::vector<std::unique_ptr<GuiElement>> result;

    for (auto &child : node.GetChildren())
    {
        if (child->GetName() == Gui::TEXT)
        {
            result.push_back(ReadGuiText(*child));
        }
        else if (child->GetName() == Gui::TEXTURE)
        {
            result.push_back(ReadGuiTexture(*child));
        }
        else if (child->GetName() == Gui::BUTTON)
        {
            result.push_back(ReadGuiButton(*child));
        }
        else if (child->GetName() == Gui::EDIT_BOX)
        {
            result.push_back(ReadEditBox(*child));
        }
        else if (child->GetName() == Gui::HORIZONTAL_LAYOUT)
        {
            result.push_back(ReadHorizontalLayout(*child));
        }
        else if (child->GetName() == Gui::VERTICAL_LAYOUT)
        {
            auto vlayout = ReadVerticalLayout(*child);
            if (vlayout)
            {
                result.push_back(std::move(vlayout));
            }
            else
            {
                ERROR_LOG("read vertical layout error.");
            }
        }
        else if (child->GetName() == Gui::WINDOW)
        {
            auto window = ReadGuiWindow(*child);
            if (window)
            {
                result.push_back(std::move(window));
            }
            else
            {
                ERROR_LOG("read window error.");
            }
        }
        else if (child->GetName() == Gui::TREE_VIEW)
        {
            auto treeView = ReadTreeView(*child);
            if (treeView)
            {
                result.push_back(std::move(treeView));
            }
            else
            {
                ERROR_LOG("read tree view error.");
            }
        }
        else
        {
            ERROR_LOG("try read unknown gui element type : " + child->GetName());
            continue;
        }
    }
    return result;
}

std::unique_ptr<VerticalLayout> GuiElementReader::ReadVerticalLayout(Utils::XmlNode &node)
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

std::unique_ptr<HorizontalLayout> GuiElementReader::ReadHorizontalLayout(Utils::XmlNode &node)
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

std::unique_ptr<GuiWindowElement> GuiElementReader::ReadGuiWindow(Utils::XmlNode &node)
{
    std::string background;
    auto paramNode = node.GetChild(Gui::BACKGROUND);
    if (paramNode)
    {
        background = paramNode->value_;
    }

    paramNode = node.GetChild(Gui::SCALE);

    vec2 position(0.f), scale(0.25f);
    if (paramNode)
    {
        scale = Utils::ReadVec2(*paramNode);
    }

    paramNode = node.GetChild(Gui::POSITION);
    if (paramNode)
    {
        position = Utils::ReadVec2(*paramNode);
    }

    std::string style;
    paramNode = node.GetChild(Gui::STYLE);
    if (paramNode)
    {
        style = paramNode->value_;
    }

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
