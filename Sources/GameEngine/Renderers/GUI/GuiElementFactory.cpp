#include "GuiElementFactory.h"
#include <algorithm>
#include "Button/GuiButton.h"
#include "EditText/GuiEditText.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GuiManager.h"
#include "Layout/HorizontalLayout.h"
#include "Layout/VerticalLayout.h"
#include "Text/GuiTextElement.h"
#include "Text/GuiTextFactory.h"
#include "Texutre/GuiTextureElement.h"
#include "Utils/XML/XMLUtils.h"
#include "Utils/XML/XmlReader.h"
#include "Utils/md5.h"
#include "Window/GuiWindow.h"

namespace GameEngine
{
GuiElementFactory::GuiElementFactory(GuiElementFactory::EntryParameters &entryParameters)
    : guiManager_(entryParameters.guiManager_)
    , inputManager_(entryParameters.inputManager_)
    , resourceManager_(entryParameters.resourceManager_)
    , renderersManager_(entryParameters.renderersManager_)
    , windowSize_(entryParameters.renderersManager_.GetProjection().GetRenderingSize())
    , guiTextFactory_(entryParameters.resourceManager_, EngineConf.renderer.resolution)
    , unsubscribe_([this](uint32 id) { renderersManager_.GetGuiRenderer().UnSubscribe(id); })
    , guiElementCounter_(0)
{
}

GuiTextElement *GuiElementFactory::CreateGuiText(const std::string &font, const std::string &str, uint32 size, uint32 outline)
{
    auto text   = guiTextFactory_.Create(EngineConf_GetFullDataPathAddToRequierd(font), str, size, outline);
    auto result = text.get();
    guiManager_.Add(std::move(text));
    return result;
}

GuiTextElement *GuiElementFactory::CreateGuiText(const std::string &text)
{
    return CreateGuiText(theme_.font, text, theme_.fontSize_, theme_.fontOutline);
}

GuiTextureElement *GuiElementFactory::CreateGuiTexture(const std::string &filename)
{
    auto guiTexture = MakeGuiTexture(filename);
    if (not guiTexture)
    {
        return nullptr;
    }

    auto result = guiTexture.get();
    guiManager_.Add(std::move(guiTexture));
    return result;
}

GuiWindowElement *GuiElementFactory::CreateGuiWindow(const Rect &rect)
{
    auto result = CreateGuiWindow(vec2(), vec2());
    result->SetRect(rect);
    return result;
}

GuiWindowElement *GuiElementFactory::CreateGuiWindow(const Rect & rect, const std::string& backgorund)
{
    auto result = CreateGuiWindow(vec2(), vec2(), backgorund);
    result->SetRect(rect);
    return result;
}

GuiWindowElement *GuiElementFactory::CreateGuiWindow(const vec2 &position, const vec2 &scale)
{
    return CreateGuiWindow(position, scale, theme_.backgroundTexture);
}

GuiWindowElement *GuiElementFactory::CreateGuiWindow(const vec2 &position, const vec2 &scale, const std::string &backgorund)
{
    auto guiWindow = std::make_unique<GuiWindowElement>(windowSize_, inputManager_);
    guiWindow->SetPostion(position);
    guiWindow->SetScale(scale);

    auto result = guiWindow.get();
    if (not backgorund.empty())
    {
        auto backgroundGuiTexture = MakeGuiTexture(backgorund);
        if (backgroundGuiTexture)
        {
            backgroundGuiTexture->SetZPosition(0.5f);
            backgroundGuiTexture->SetScale(guiWindow->GetScale());
            guiWindow->AddChild(backgroundGuiTexture.get());
            guiManager_.Add(std::move(backgroundGuiTexture));
        }
    }

    auto closeButton  = CreateGuiButton([result]() { result->MarkToRemove(); });
    auto closeButtonX = CreateGuiText(theme_.font, "X", 32, 0);
    closeButton->SetScale(closeButtonX->GetScale());
    closeButton->SetText(closeButtonX);
    auto closeButtonPosition = position + scale - closeButtonX->GetScale();
    closeButton->SetPostion(closeButtonPosition);
    guiWindow->AddChild(closeButton);

    guiManager_.Add(std::move(guiWindow));
    return result;
}

GuiButtonElement *GuiElementFactory::CreateGuiButton(std::function<void()> onClick)
{
    auto isOnTop = [this](uint32 id) {
        auto mousePosition          = inputManager_.GetMousePosition();
        const auto &checkingElement = guiManager_.GetElement(id);
        float checkingZ             = checkingElement->GetZTotalValue() - 0.001f;

        for (const auto &element : guiManager_.GetElements())
        {
            if (element->GetType() == GuiElementTypes::Text or element->GetId() == checkingElement->GetId())
            {
                continue;
            }

            if (element->IsShow() and element->IsCollision(mousePosition) and element->GetZTotalValue() < checkingZ)
            {
                return false;
            }
        }
        return true;
    };

    auto button = std::make_unique<GuiButtonElement>(isOnTop, inputManager_, onClick, windowSize_);

    if (not theme_.buttonTexture.empty())
    {
        auto texture = CreateGuiTexture(theme_.buttonTexture);
        button->SetBackgroundTexture(texture);
    }
    if (not theme_.buttonHoverTexture.empty())
    {
        auto texture = CreateGuiTexture(theme_.buttonHoverTexture);
        button->SetOnHoverTexture(texture);
    }
    if (not theme_.buttonActiveTexture.empty())
    {
        auto texture = CreateGuiTexture(theme_.buttonActiveTexture);
        button->SetOnActiveTexture(texture);
    }

    auto result = button.get();
    guiManager_.Add(std::move(button));
    return result;
}

GuiButtonElement *GuiElementFactory::CreateGuiButton(const std::string &text, std::function<void()> onclick)
{
    auto button  = CreateGuiButton(onclick);
    auto guiText = CreateGuiText(text);
    button->SetScale(guiText->GetScale());
    button->SetText(guiText);
    return button;
}

GuiEditBoxElement *GuiElementFactory::CreateEditBox()
{
    auto guiText          = CreateGuiText("");
    auto editText         = CreateEditBox(guiText);
    auto editBoxBgTexture = CreateGuiTexture(theme_.editBoxBackground);
    editText->SetBackgroundTexture(editBoxBgTexture);
    return editText;
}

GuiEditBoxElement *GuiElementFactory::CreateEditBox(const std::string &font, const std::string &str, uint32 size, uint32 outline)
{
    auto text    = CreateGuiText(font, str, size, outline);
    auto editBox = std::make_unique<GuiEditBoxElement>(*text, inputManager_, windowSize_);
    auto result  = editBox.get();
    guiManager_.Add(std::move(editBox));
    return result;
}

GuiEditBoxElement *GuiElementFactory::CreateEditBox(GuiTextElement *text)
{
    auto editBox = std::make_unique<GuiEditBoxElement>(*text, inputManager_, windowSize_);
    auto result  = editBox.get();
    guiManager_.Add(std::move(editBox));
    return result;
}

VerticalLayout *GuiElementFactory::CreateVerticalLayout()
{
    auto layout = std::make_unique<VerticalLayout>(windowSize_, inputManager_, unsubscribe_);
    auto result = layout.get();
    guiManager_.Add(std::move(layout));
    return result;
}

HorizontalLayout *GuiElementFactory::CreateHorizontalLayout()
{
    auto layout = std::make_unique<HorizontalLayout>(windowSize_, inputManager_, unsubscribe_);
    auto result = layout.get();
    guiManager_.Add(std::move(layout));
    return result;
}

void GuiElementFactory::CreateMessageBox(const std::string &title, const std::string &message, std::function<void()> okFunc)
{
    auto window = CreateGuiWindow(vec2(0, 0), vec2(0.5, 0.3));
    window->SetZPosition(-100.f);

    auto titleText = CreateGuiText(title);
    titleText->SetPostion(vec2(0, 0.275));
    window->AddChild(titleText);

    auto messageText = CreateGuiText(message);
    window->AddChild(messageText);

    auto button = CreateGuiButton("ok", [window, okFunc]() {
        window->MarkToRemove();
        if (okFunc)
            okFunc();
    });

    button->SetScale(1.5f * button->GetScale());
    button->SetPostion(vec2(0, -0.25));
    window->AddChild(button);
}

void GuiElementFactory::SetTheme(const GuiTheme &theme)
{
    theme_ = theme;
}

const GuiTheme &GuiElementFactory::GetTheme() const
{
    return theme_;
}

std::unique_ptr<GuiTextureElement> GuiElementFactory::MakeGuiTexture(const std::string &filename)
{
    auto texture = resourceManager_.GetTextureLaoder().LoadTexture(filename, false, true, ObjectTextureType::MATERIAL, TextureFlip::Type::VERTICAL);
    if (not texture)
    {
        DEBUG_LOG("Texture not loaded : " + filename);
        return nullptr;
    }

    return std::make_unique<GuiTextureElement>(windowSize_, *texture);
}

void ReadGuiElementBasic(GuiElement *element, Utils::XmlNode &node)
{
    if (not element)
        return;
    /*
    vec2ui windowSize_;
    Rect rect_;
    vec2 position_;
    float zPosition_;
    vec2 scale_;
    vec3 color_;
    mat4 transformMatrix_;
    float rotation_;
    bool show_;
    */

    auto paramNode = node.GetChild("color");
    if (paramNode)
    {
        auto color = Utils::ReadVec3(*paramNode);
        element->SetColor(color);
    }

    paramNode = node.GetChild("inBackground");
    if (paramNode)
    {
        auto inBackground = Utils::ReadBool(*paramNode);
        if (inBackground)
            element->SetZPosition(0.1f);
    }

    paramNode = node.GetChild("position");
    if (paramNode)
    {
        auto position = Utils::ReadVec2(*paramNode);
        element->SetPostion(position);
    }
    paramNode = node.GetChild("show");
    if (paramNode)
    {
        element->Show(Utils::ReadBool(*paramNode));
    }

    paramNode = node.GetChild("scale");
    if (paramNode)
    {
        element->SetScale(Utils::ReadVec2(*paramNode));
    }

    paramNode = node.GetChild("label");
    if (paramNode)
    {
        element->SetLabel(paramNode->value_);
    }
}

GuiTextElement *ReadGuiText(Utils::XmlNode &node, GuiElementFactory &factory)
{
    if (node.GetName() != "text")
    {
        return nullptr;
    }
    std::string font = "GUI/Ubuntu-M.ttf", value = "empty string";
    uint32 fontSize = 10, outline = 0;

    auto paramNode = node.GetChild("font");
    if (paramNode)
    {
        font = paramNode->value_;
    }

    paramNode = node.GetChild("value");
    if (paramNode)
    {
        value = paramNode->value_;
    }

    paramNode = node.GetChild("fontSize");
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

    paramNode = node.GetChild("outline");
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

    auto text = factory.CreateGuiText(font, value, fontSize, outline);

    ReadGuiElementBasic(text, node);
    return text;
}

GuiTextureElement *ReadGuiTexture(Utils::XmlNode &node, GuiElementFactory &factory)
{
    if (node.GetName() != "texture")
    {
        return nullptr;
    }

    std::string filename;

    auto paramNode = node.GetChild("file");

    if (paramNode)
    {
        filename = paramNode->value_;
    }

    auto texture = factory.CreateGuiTexture(filename);

    if (not texture)
    {
        return nullptr;
    }

    ReadGuiElementBasic(texture, node);
    return texture;
}
GuiButtonElement *ReadGuiButton(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manager)
{
    if (node.GetName() != "button")
    {
        return nullptr;
    }

    std::function<void()> onClick = []() {};

    auto paramNode = node.GetChild("action");
    if (paramNode)
    {
        onClick = manager.GetActionFunction(paramNode->value_);
    }

    auto button = factory.CreateGuiButton(onClick);

    //auto &children             = node.GetChildren();
//    auto backgroundTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
//        auto c = child->GetChild("label");
//        if (c)
//        {
//            return c->value_ == "backgroundTexture";
//        }
//        return false;
//    });

//    if (backgroundTextureNode != children.end())
//    {
//        auto texture = ReadGuiTexture(**backgroundTextureNode, factory);

//        if (texture)
//            button->SetBackgroundTexture(texture);
//    }

//    auto hoverTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
//        auto c = child->GetChild("label");
//        if (c)
//        {
//            return c->value_ == "hoverTexture";
//        }
//        return false;
//    });
//    if (hoverTextureNode != children.end())
//    {
//        auto texture = ReadGuiTexture(**hoverTextureNode, factory);
//        if (texture)
//            button->SetOnHoverTexture(texture);
//    }

//    auto activeTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
//        auto c = child->GetChild("label");
//        if (c)
//        {
//            return c->value_ == "activeTexture";
//        }
//        return false;
//    });

//    if (hoverTextureNode != children.end())
//    {
//        auto texture = ReadGuiTexture(**activeTextureNode, factory);
//        if (texture)
//            button->SetOnActiveTexture(texture);
//    }

    auto textNode = node.GetChild("text");
    if (textNode)
    {
        auto text = ReadGuiText(*textNode, factory);
        if (text)
        {
            button->SetText(text);
        }
    }

    ReadGuiElementBasic(button, node);
    return button;
}
GuiEditBoxElement *ReadEditBox(Utils::XmlNode &node, GuiElementFactory &factory)
{
    if (node.GetName() != "editBox")
    {
        return nullptr;
    }
    auto textNode = node.GetChild("text");
    if (textNode)
    {
        auto text    = ReadGuiText(*textNode, factory);
        auto editBox = factory.CreateEditBox(text);
        ReadGuiElementBasic(editBox, node);
        return editBox;
    }

    return nullptr;
}

VerticalLayout *ReadVerticalLayout(Utils::XmlNode &, GuiElementFactory &, GuiManager &);
HorizontalLayout *ReadHorizontalLayout(Utils::XmlNode &, GuiElementFactory &, GuiManager &);

void ReadLayoutChildren(Utils::XmlNode &node, GuiElementFactory &factory, Layout *layout, GuiManager &manager)
{
    for (auto &child : node.GetChildren())
    {
        DEBUG_LOG("Node : " + child->GetName());

        if (auto text = ReadGuiText(*child, factory))
        {
            layout->AddChild(text);
        }
        else if (auto texture = ReadGuiTexture(*child, factory))
        {
            layout->AddChild(texture);
        }
        else if (auto button = ReadGuiButton(*child, factory, manager))
        {
            layout->AddChild(button);
        }
        else if (auto editBox = ReadEditBox(*child, factory))
        {
            layout->AddChild(editBox);
        }
        else if (auto horizontalLayout = ReadHorizontalLayout(*child, factory, manager))
        {
            layout->AddChild(horizontalLayout);
        }
        else if (auto verticalLayout = ReadVerticalLayout(*child, factory, manager))
        {
            layout->AddChild(verticalLayout);
        }
    }
}

VerticalLayout *ReadVerticalLayout(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger)
{
    if (node.GetName() != "verticalLayout")
    {
        return nullptr;
    }
    auto layout = factory.CreateVerticalLayout();
    ReadGuiElementBasic(layout, node);
    ReadLayoutChildren(node, factory, layout, manger);

    auto paramNode = node.GetChild("algin");
    if (paramNode)
    {
        if (paramNode->value_ == "LEFT")
        {
            layout->SetAlgin(VerticalLayout::Algin::LEFT);
        }
        else if (paramNode->value_ == "CENTER")
        {
            layout->SetAlgin(VerticalLayout::Algin::CENTER);
        }
        else if (paramNode->value_ == "RIGHT")
        {
            layout->SetAlgin(VerticalLayout::Algin::RIGHT);
        }
    }
    return layout;
}

HorizontalLayout *ReadHorizontalLayout(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger)
{
    if (node.GetName() != "horizontalLayout")
    {
        return nullptr;
    }

    auto layout = factory.CreateHorizontalLayout();
    ReadGuiElementBasic(layout, node);
    ReadLayoutChildren(node, factory, layout, manger);
    return layout;
}

GuiWindowElement *ReadGuiWindow(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger)
{
    if (node.GetName() != "window")
    {
        return nullptr;
    }

    std::string background;

    auto paramNode = node.GetChild("background");

    if (paramNode)
    {
        background = paramNode->value_;
    }

    paramNode = node.GetChild("scale");

    vec2 position(0.f), scale(0.25f);
    if (paramNode)
    {
        scale = Utils::ReadVec2(*paramNode);
    }

    paramNode = node.GetChild("position");
    if (paramNode)
    {
        position = Utils::ReadVec2(*paramNode);
    }

    auto window = factory.CreateGuiWindow(position, scale);

    for (auto &child : node.GetChildren())
    {
        DEBUG_LOG("Node : " + child->GetName());

        if (auto text = ReadGuiText(*child, factory))
        {
            window->AddChild(text);
        }
        else if (auto texture = ReadGuiTexture(*child, factory))
        {
            window->AddChild(texture);
        }
        else if (auto button = ReadGuiButton(*child, factory, manger))
        {
            window->AddChild(button);
        }
        else if (auto editBox = ReadEditBox(*child, factory))
        {
            window->AddChild(editBox);
        }
        else if (auto verticalLayout = ReadVerticalLayout(*child, factory, manger))
        {
            window->AddChild(verticalLayout);
        }
        else if (auto horizontalLayout = ReadHorizontalLayout(*child, factory, manger))
        {
            window->AddChild(horizontalLayout);
        }
    }

    ReadGuiElementBasic(window, node);
    return window;
}

void ReadTheme(Utils::XmlNode &node, GuiElementFactory &factory)
{
    if (node.GetName() != "theme")
    {
        return;
    }

    GuiTheme theme;
    for (auto &child : node.GetChildren())
    {
        if (child->GetName() == "backgroundTexture")
        {
            theme.backgroundTexture = child->value_;
        }
        else if (child->GetName() == "buttonTexture")
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
    factory.SetTheme(theme);
}

bool GuiElementFactory::ReadGuiFile(const std::string &filename)
{
    if (not Utils::CheckExtension(filename, "xml"))
    {
        ERROR_LOG("This is not xml file. Format should be \".xml\".");
        return false;
    }

    auto fileContent = Utils::ReadFile(filename);

    if (fileContent.empty())
    {
        return false;
    }

    auto md5Value = md5(fileContent);

    if (md5Value == lastGuiFileMd5Value_)
    {
        return false;
    }

    DEBUG_LOG("Gui file changed. Parsing : " + filename);
    lastGuiFileMd5Value_ = md5Value;

    Utils::XmlReader reader;
    if (not reader.ReadXml(fileContent))
    {
        return false;
    }

    auto guiNode = reader.Get("gui");
    if (not guiNode)
    {
        ERROR_LOG("This is not gui file.");
        return false;
    }

    for (auto &node : guiNode->GetChildren())
    {
        DEBUG_LOG("Node : " + node->GetName());

        ReadTheme(*node, *this);
        ReadHorizontalLayout(*node, *this, guiManager_);
        ReadVerticalLayout(*node, *this, guiManager_);
        ReadGuiText(*node, *this);
        ReadGuiTexture(*node, *this);
        ReadGuiWindow(*node, *this, guiManager_);
        ReadGuiButton(*node, *this, guiManager_);
        ReadEditBox(*node, *this);
    }
    return true;
}
}  // namespace GameEngine
