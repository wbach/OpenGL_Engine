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
    , renderSubscribe_([this](GuiElement &element) { renderersManager_.GetGuiRenderer().Subscribe(element); })
    , unsubscribeElement_([this](const GuiElement &element) { renderersManager_.GetGuiRenderer().UnSubscribe(element); })
    , guiTextFactory_(renderSubscribe_, unsubscribeElement_, entryParameters.resourceManager_, EngineConf.renderer.resolution)
    , guiElementCounter_(0)
{
    isOnTop_ = [this](GuiElement &checkingElement) {
        auto mousePosition = inputManager_.GetMousePosition();
        for (const auto &layer : guiManager_.GetGuiLayers())
        {
            for (const auto &element : layer.GetElements())
            {
                if (element->GetType() == GuiElementTypes::Text or element->GetId() == element->GetId())
                {
                    continue;
                }

                if (element->IsShow() and element->IsCollision(mousePosition) and element->CompareZValue(checkingElement))
                {
                    return false;
                }
            }
        }
        return true;
    };
}

std::unique_ptr<GuiTextElement> GuiElementFactory::CreateGuiText(const std::string &font, const std::string &str, uint32 size, uint32 outline)
{
    auto result = guiTextFactory_.Create(EngineConf_GetFullDataPathAddToRequierd(font), str, size, outline);
    return result;
}

std::unique_ptr<GuiTextElement> GuiElementFactory::CreateGuiText(const std::string &text)
{
    return CreateGuiText(theme_.font, text, theme_.fontSize_, theme_.fontOutline);
}

std::unique_ptr<GuiTextureElement> GuiElementFactory::CreateGuiTexture(const std::string &filename)
{
    auto texture = MakeGuiTexture(filename);
    return texture;
}

std::unique_ptr<GuiWindowElement> GuiElementFactory::CreateGuiWindow(const Rect &rect)
{
    auto result = CreateGuiWindow(vec2(), vec2());
    result->SetRect(rect);
    return result;
}

std::unique_ptr<GuiWindowElement> GuiElementFactory::CreateGuiWindow(const Rect &rect, const std::string &backgorund)
{
    auto result = CreateGuiWindow(vec2(), vec2(), backgorund);
    result->SetRect(rect);
    return result;
}

std::unique_ptr<GuiWindowElement> GuiElementFactory::CreateGuiWindow(const vec2 &position, const vec2 &scale)
{
    return CreateGuiWindow(position, scale, theme_.backgroundTexture);
}

std::unique_ptr<GuiWindowElement> GuiElementFactory::CreateGuiWindow(const vec2 &position, const vec2 &scale, const std::string &backgorund)
{
    auto guiWindow = std::make_unique<GuiWindowElement>(windowSize_, inputManager_);
    guiWindow->SetPostion(position);
    guiWindow->SetScale(scale);

    if (not backgorund.empty())
    {
        auto backgroundGuiTexture = MakeGuiTexture(backgorund);
        if (backgroundGuiTexture)
        {
            backgroundGuiTexture->SetZPosition(0.5f);
            backgroundGuiTexture->SetScale(guiWindow->GetScale());
            guiWindow->AddChild(std::move(backgroundGuiTexture));
        }
    }

    auto guiWindowPtr = guiWindow.get();

    auto closeButton  = CreateGuiButton([guiWindowPtr](auto &) { guiWindowPtr->MarkToRemove(); });
    auto closeButtonX = CreateGuiText(theme_.font, "X", 32, 0);
    closeButton->SetScale(closeButtonX->GetScale());
    auto closeButtonPosition = position + scale + closeButtonX->GetScale();
    closeButton->SetPostion(closeButtonPosition);

    closeButton->SetText(std::move(closeButtonX));
    guiWindow->AddChild(std::move(closeButton));
    return guiWindow;
}

std::unique_ptr<GuiButtonElement> GuiElementFactory::CreateGuiButton(std::function<void(GuiElement &)> onClick)
{
    auto button = std::make_unique<GuiButtonElement>(isOnTop_, inputManager_, onClick, windowSize_);

    if (not theme_.buttonTexture.empty())
    {
        auto texture = CreateGuiTexture(theme_.buttonTexture);
        button->SetBackgroundTexture(std::move(texture));
    }
    if (not theme_.buttonHoverTexture.empty())
    {
        auto texture = CreateGuiTexture(theme_.buttonHoverTexture);
        button->SetOnHoverTexture(std::move(texture));
    }
    if (not theme_.buttonActiveTexture.empty())
    {
        auto texture = CreateGuiTexture(theme_.buttonActiveTexture);
        button->SetOnActiveTexture(std::move(texture));
    }

    return button;
}

std::unique_ptr<GuiButtonElement> GuiElementFactory::CreateGuiButton(const std::string &text, std::function<void(GuiElement &)> onclick)
{
    auto button  = CreateGuiButton(onclick);
    auto guiText = CreateGuiText(text);
    button->SetScale(guiText->GetScale());
    button->SetText(std::move(guiText));
    return button;
}

std::unique_ptr<GuiEditBoxElement> GuiElementFactory::CreateEditBox()
{
    auto guiText = CreateGuiText("");
    return CreateEditBox(std::move(guiText));
}

std::unique_ptr<GuiEditBoxElement> GuiElementFactory::CreateEditBox(const std::string &font, const std::string &str, uint32 size, uint32 outline)
{
    auto text = CreateGuiText(font, str, size, outline);
    return CreateEditBox(std::move(text));
}

std::unique_ptr<GuiEditBoxElement> GuiElementFactory::CreateEditBox(std::unique_ptr<GuiTextElement> text)
{
    auto cursor           = CreateGuiText("|");
    auto editBox          = std::make_unique<GuiEditBoxElement>(std::move(text), std::move(cursor), inputManager_, windowSize_);
    auto editBoxBgTexture = CreateGuiTexture(theme_.editBoxBackground);
    editBox->SetBackgroundTexture(std::move(editBoxBgTexture));
    return editBox;
}

std::unique_ptr<VerticalLayout> GuiElementFactory::CreateVerticalLayout()
{
    return std::make_unique<VerticalLayout>(windowSize_, inputManager_);
}

std::unique_ptr<HorizontalLayout> GuiElementFactory::CreateHorizontalLayout()
{
    return std::make_unique<HorizontalLayout>(windowSize_, inputManager_);
}

void GuiElementFactory::CreateMessageBox(const std::string &title, const std::string &message, std::function<void()> okFunc)
{
    auto window = CreateGuiWindow(vec2(0, 0), vec2(0.5, 0.3));
    window->SetZPosition(-100.f);

    auto titleText = CreateGuiText(title);
    titleText->SetPostion(vec2(0, 0.275));
    window->AddChild(std::move(titleText));

    auto messageText = CreateGuiText(message);
    window->AddChild(std::move(messageText));

    auto windowPtr = window.get();
    auto button    = CreateGuiButton("ok", [windowPtr, okFunc](auto &) {
        windowPtr->MarkToRemove();
        if (okFunc)
            okFunc();
    });

    button->SetScale(1.5f * button->GetScale());
    button->SetPostion(vec2(0, -0.25));
    window->AddChild(std::move(button));
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

    return std::make_unique<GuiTextureElement>(renderSubscribe_, unsubscribeElement_, windowSize_, *texture);
}

void ReadGuiElementBasic(GuiElement &element, Utils::XmlNode &node)
{
    auto paramNode = node.GetChild("color");
    if (paramNode)
    {
        auto color = Utils::ReadVec3(*paramNode);
        element.SetColor(color);
    }

    paramNode = node.GetChild("inBackground");
    if (paramNode)
    {
        auto inBackground = Utils::ReadBool(*paramNode);
        if (inBackground)
        {
            element.SetZPosition(0.1f);
            element.SetInBackgorund(true);
        }
    }

    paramNode = node.GetChild("position");
    if (paramNode)
    {
        auto position = Utils::ReadVec2(*paramNode);
        element.SetPostion(position);
    }
    paramNode = node.GetChild("show");
    if (paramNode)
    {
        element.Show(Utils::ReadBool(*paramNode));
    }

    paramNode = node.GetChild("scale");
    if (paramNode)
    {
        element.SetScale(Utils::ReadVec2(*paramNode));
    }

    paramNode = node.GetChild("label");
    if (paramNode)
    {
        element.SetLabel(paramNode->value_);
    }

    paramNode = node.GetChild("permamanet");
    if (paramNode)
    {
        element.SetPermamanet(Utils::ReadBool(*paramNode));
    }
    else
    {
        element.SetPermamanet(false);
    }
}

std::unique_ptr<GuiTextElement> ReadGuiText(Utils::XmlNode &node, GuiElementFactory &factory)
{
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
    ReadGuiElementBasic(*text, node);
    return text;
}

std::unique_ptr<GuiTextureElement> ReadGuiTexture(Utils::XmlNode &node, GuiElementFactory &factory)
{
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

    ReadGuiElementBasic(*texture, node);
    return texture;
}
std::unique_ptr<GuiButtonElement> ReadGuiButton(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manager)
{
    ActionFunction onClick;
    auto paramNode = node.GetChild("action");
    if (paramNode)
    {
        onClick = manager.GetActionFunction(paramNode->value_);
    }
    auto button                = factory.CreateGuiButton(onClick);
    auto &children             = node.GetChildren();
    auto backgroundTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->GetChild("label");
        if (c)
        {
            return c->value_ == "backgroundTexture";
        }
        return false;
    });

    if (backgroundTextureNode != children.end())
    {
        auto texture = ReadGuiTexture(**backgroundTextureNode, factory);
        if (texture)
            button->SetBackgroundTexture(std::move(texture));
    }

    auto hoverTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->GetChild("label");
        if (c)
        {
            return c->value_ == "hoverTexture";
        }
        return false;
    });
    if (hoverTextureNode != children.end())
    {
        auto texture = ReadGuiTexture(**hoverTextureNode, factory);
        if (texture)
            button->SetOnHoverTexture(std::move(texture));
    }

    auto activeTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->GetChild("label");
        if (c)
        {
            return c->value_ == "activeTexture";
        }
        return false;
    });

    if (hoverTextureNode != children.end())
    {
        auto texture = ReadGuiTexture(**activeTextureNode, factory);
        if (texture)
            button->SetOnActiveTexture(std::move(texture));
    }

    auto textNode = node.GetChild("text");
    if (textNode)
    {
        auto text = ReadGuiText(*textNode, factory);
        if (text)
        {
            button->SetText(std::move(text));
        }
    }
    ReadGuiElementBasic(*button, node);
    return button;
}
std::unique_ptr<GuiEditBoxElement> ReadEditBox(Utils::XmlNode &node, GuiElementFactory &factory)
{
    auto textNode = node.GetChild("text");
    if (textNode)
    {
        auto text    = ReadGuiText(*textNode, factory);
        auto editBox = factory.CreateEditBox(std::move(text));
        ReadGuiElementBasic(*editBox, node);
        return editBox;
    }

    return nullptr;
}

std::unique_ptr<VerticalLayout> ReadVerticalLayout(Utils::XmlNode &, GuiElementFactory &, GuiManager &);
std::unique_ptr<HorizontalLayout> ReadHorizontalLayout(Utils::XmlNode &, GuiElementFactory &, GuiManager &);
std::unique_ptr<GuiWindowElement> ReadGuiWindow(Utils::XmlNode &, GuiElementFactory &, GuiManager &);
std::vector<std::unique_ptr<GuiElement>> ReadChildrenElemets(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manager)
{
    std::vector<std::unique_ptr<GuiElement>> result;

    for (auto &child : node.GetChildren())
    {
        if (child->GetName() == "text")
        {
            result.push_back(ReadGuiText(*child, factory));
        }
        else if (child->GetName() == "texture")
        {
            result.push_back(ReadGuiTexture(*child, factory));
        }
        else if (child->GetName() == "button")
        {
            result.push_back(ReadGuiButton(*child, factory, manager));
        }
        else if (child->GetName() == "editBox")
        {
            result.push_back(ReadEditBox(*child, factory));
        }
        else if (child->GetName() == "horizontalLayout")
        {
            result.push_back(ReadHorizontalLayout(*child, factory, manager));
        }
        else if (child->GetName() == "verticalLayout")
        {
            result.push_back(ReadVerticalLayout(*child, factory, manager));
        }
        else if (child->GetName() == "window")
        {
            result.push_back(ReadGuiWindow(*child, factory, manager));
        }
        else
        {
            ERROR_LOG("try read unknown gui element type : " + child->GetName());
        }
    }

    return result;
}

std::unique_ptr<VerticalLayout> ReadVerticalLayout(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger)
{
    auto layout = factory.CreateVerticalLayout();
    ReadGuiElementBasic(*layout, node);

    auto children = ReadChildrenElemets(node, factory, manger);
    for (auto &child : children)
    {
        layout->AddChild(std::move(child));
    }

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

std::unique_ptr<HorizontalLayout> ReadHorizontalLayout(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger)
{
    auto layout = factory.CreateHorizontalLayout();
    ReadGuiElementBasic(*layout, node);
    auto children = ReadChildrenElemets(node, factory, manger);
    for (auto &child : children)
    {
        layout->AddChild(std::move(child));
    }

    return layout;
}

std::unique_ptr<GuiWindowElement> ReadGuiWindow(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manager)
{
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
    ReadGuiElementBasic(*window, node);

    auto children = ReadChildrenElemets(node, factory, manager);
    for (auto &child : children)
    {
        window->AddChild(std::move(child));
    }

    return window;
}

GuiTheme ReadTheme(Utils::XmlNode &node)
{
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
    return theme;
}

bool GuiElementFactory::ReadGuiFile(const std::string &filename)
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
        return false;
    }

    guiManager_.RemoveNotPermaments();

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

    auto theme = guiNode->GetChild("theme");

    if (theme)
    {
        SetTheme(ReadTheme(*theme));
    }

    auto children = ReadChildrenElemets(*guiNode, *this, guiManager_);

    for (auto &child : children)
    {
        guiManager_.Add(std::move(child));
    }
    return true;
}
}  // namespace GameEngine
