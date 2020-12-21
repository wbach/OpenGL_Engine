#include "GuiElementFactory.h"

#include <Logger/Log.h>

#include <algorithm>

#include "Button/GuiButton.h"
#include "EditText/GuiEditText.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GuiElementReader.h"
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
GuiElementFactory::GuiElementFactory(GuiElementFactory::EntryParameters &entryParameters)
    : guiManager_(entryParameters.guiManager_)
    , guiRenderer_(entryParameters.renderersManager_.GetGuiRenderer())
    , inputManager_(entryParameters.inputManager_)
    , resourceManager_(entryParameters.resourceManager_)
    , renderersManager_(entryParameters.renderersManager_)
    , windowSize_(*EngineConf.window.size)
    , fontManger_(windowSize_)
    , guiElementCounter_(0)
{
    isOnTop_ = [this](const GuiElement &checkingElement) {
        auto mousePosition = inputManager_.GetMousePosition();
        for (const auto &layer : guiManager_.GetGuiLayers())
        {
            for (const auto &element : layer.GetElements())
            {
                if (element->GetType() == GuiElementTypes::Text or element->GetId() == element->GetId())
                {
                    continue;
                }

                if (element->IsShow() and element->IsCollision(mousePosition) and
                    element->CompareZValue(checkingElement))
                {
                    return false;
                }
            }
        }
        return true;
    };
}

GuiElementFactory::~GuiElementFactory()
{
    DEBUG_LOG("destructor");
}

std::unique_ptr<GuiTextElement> GuiElementFactory::CreateGuiText(const std::string &font, const std::string &text,
                                                                 uint32 fontSize, uint32 outline)
{
    auto textElement = std::make_unique<GuiTextElement>(fontManger_, guiRenderer_, resourceManager_, windowSize_, font,
                                                        text, fontSize, outline);
    return textElement;
}

std::unique_ptr<GuiTextElement> GuiElementFactory::CreateGuiText(const std::string &text, uint32 fontSize)
{
    return CreateGuiText(theme_.font, text, fontSize, theme_.fontOutline);
}

std::unique_ptr<GuiTextElement> GuiElementFactory::CreateGuiText(const std::string &text)
{
    return CreateGuiText(theme_.font, text, theme_.fontSize_, theme_.fontOutline);
}

std::unique_ptr<GuiTextureElement> GuiElementFactory::CreateGuiTexture(const std::string &filename)
{
    return MakeGuiTexture(filename);
}

std::unique_ptr<GuiWindowElement> GuiElementFactory::CreateGuiWindow(GuiWindowStyle style, const vec2 &position,
                                                                     const vec2 &scale)
{
    return CreateGuiWindow(style, position, scale, theme_.backgroundTexture);
}

std::unique_ptr<GuiWindowElement> GuiElementFactory::CreateGuiWindow(GuiWindowStyle style, const vec2 &position,
                                                                     const vec2 &scale, const vec4 &backgorundColor)
{
    return CreateGuiWindow(style, position, scale, theme_.backgroundTexture, backgorundColor);
}

std::unique_ptr<GuiWindowElement> GuiElementFactory::CreateGuiWindow(GuiWindowStyle style, const vec2 &position,
                                                                     const vec2 &scale, const std::string &backgorund)
{
    return CreateGuiWindow(style, position, scale, backgorund, vec4(1.f));
}

std::unique_ptr<GuiWindowElement> GuiElementFactory::CreateGuiWindow(GuiWindowStyle style, const vec2 &position,
                                                                     const vec2 &scale, const std::string &backgorund,
                                                                     const vec4 &backgorundColor)
{
    auto guiWindow = std::make_unique<GuiWindowElement>(style, windowSize_, inputManager_);
    guiWindow->SetPostion(position);
    guiWindow->SetScale(scale);

    if (style != GuiWindowStyle::EMPTY)
    {
        auto &bg                  = backgorund.empty() ? theme_.backgroundTexture : backgorund;
        auto backgroundGuiTexture = MakeGuiTexture(bg);

        if (backgroundGuiTexture)
        {
            backgroundGuiTexture->SetColor(backgorundColor);
            guiWindow->SetBackground(std::move(backgroundGuiTexture));
        }
    }

    if (style != GuiWindowStyle::EMPTY and style != GuiWindowStyle::BACKGROUND_ONLY)
    {
        CreateWindowBar(style, *guiWindow);
    }
    return guiWindow;
}

std::unique_ptr<GuiButtonElement> GuiElementFactory::CreateGuiButton(std::function<void(GuiElement &)> onClick)
{
    auto button = std::make_unique<GuiButtonElement>(isOnTop_, inputManager_, onClick, windowSize_);
    return CreateGuiButton(onClick, theme_.buttonTexture, theme_.buttonHoverTexture, theme_.buttonActiveTexture);
}

std::unique_ptr<GuiButtonElement> GuiElementFactory::CreateGuiButton(const std::string &text,
                                                                     std::function<void(GuiElement &)> onclick)
{
    auto button  = CreateGuiButton(onclick);
    auto guiText = CreateGuiText(text);
    button->SetScale(guiText->GetScale());
    button->SetText(std::move(guiText));
    return button;
}

std::unique_ptr<GuiButtonElement> GuiElementFactory::CreateGuiButton(std::function<void(GuiElement &)> onClick,
                                                                     const std::string &bgtexture,
                                                                     const std::string &hoverTexture,
                                                                     const std::string &activeTexture)
{
    auto button = std::make_unique<GuiButtonElement>(isOnTop_, inputManager_, onClick, windowSize_);

    if (not bgtexture.empty())
    {
        auto texture = CreateGuiTexture(bgtexture);
        if (texture)
        {
            button->SetBackgroundTexture(std::move(texture));
        }
    }
    if (not hoverTexture.empty())
    {
        auto texture = CreateGuiTexture(hoverTexture);
        if (texture)
        {
            button->SetOnHoverTexture(std::move(texture));
        }
    }
    if (not activeTexture.empty())
    {
        auto texture = CreateGuiTexture(activeTexture);
        if (texture)
        {
            button->SetOnActiveTexture(std::move(texture));
        }
    }

    return button;
}

std::unique_ptr<GuiButtonElement> GuiElementFactory::CreateGuiButton(const std::string &text,
                                                                     std::function<void(GuiElement &)> onclick,
                                                                     const std::string &bgtexture,
                                                                     const std::string &hoverTexture,
                                                                     const std::string &activeTexture)
{
    auto button  = CreateGuiButton(onclick, bgtexture, hoverTexture, activeTexture);
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

std::unique_ptr<GuiEditBoxElement> GuiElementFactory::CreateEditBox(const std::string &startText)
{
    auto guiText = CreateGuiText(startText);
    return CreateEditBox(std::move(guiText));
}

std::unique_ptr<GuiEditBoxElement> GuiElementFactory::CreateEditBox(const std::string &font, const std::string &str,
                                                                    uint32 size, uint32 outline)
{
    auto text = CreateGuiText(font, str, size, outline);
    return CreateEditBox(std::move(text));
}

std::unique_ptr<GuiEditBoxElement> GuiElementFactory::CreateEditBox(std::unique_ptr<GuiTextElement> text)
{
    auto cursor  = CreateGuiText("|");
    auto editBox = std::make_unique<GuiEditBoxElement>(std::move(text), std::move(cursor), inputManager_, windowSize_);
    auto editBoxBgTexture = CreateGuiTexture(theme_.editBoxBackground);
    if (editBoxBgTexture)
    {
        editBox->SetBackgroundTexture(std::move(editBoxBgTexture));
    }
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

std::unique_ptr<TreeView> GuiElementFactory::CreateTreeView(std::function<void(GuiElement &)> action)
{
    return std::make_unique<TreeView>(*this, action, windowSize_);
}

void GuiElementFactory::CreateMessageBox(const std::string &title, const std::string &message,
                                         std::function<void()> okFunc)
{
    auto window = CreateGuiWindow(GuiWindowStyle::CLOSE, vec2(0, 0), vec2(0.5, 0.3));
    window->SetZPosition(-100.f);

    auto titleText = CreateGuiText(title);
    titleText->SetPostion(vec2(0, 0.275));
    window->AddChild(std::move(titleText));

    auto messageText = CreateGuiText(message);
    window->AddChild(std::move(messageText));

    auto windowPtr = window.get();
    auto button    = CreateGuiButton("ok", [this, windowPtr, okFunc](auto &) {
        if (okFunc)
            okFunc();
        guiManager_.AddRemoveTask(windowPtr);
    });

    button->SetScale(1.5f * button->GetScale());
    button->SetPostion(vec2(0, -0.25));
    window->AddChild(std::move(button));
    guiManager_.Add(std::move(window));
}

void GuiElementFactory::SetTheme(const GuiTheme &theme)
{
    theme_ = theme;
}

const GuiTheme &GuiElementFactory::GetTheme() const
{
    return theme_;
}

std::unique_ptr<GuiTextureElement> GuiElementFactory::MakeGuiTexture(const File &file)
{
    TextureParameters params;
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    params.flipMode        = TextureFlip::VERTICAL;
    params.filter          = GraphicsApi::TextureFilter::LINEAR;
    params.mimap           = GraphicsApi::TextureMipmap::LINEAR;

    auto texture = resourceManager_.GetTextureLoader().LoadTexture(file, params);
    if (not texture)
    {
        DEBUG_LOG("Texture not loaded : " + file.GetAbsoultePath());
        return nullptr;
    }

    return std::make_unique<GuiTextureElement>(resourceManager_, guiRenderer_, windowSize_, *texture);
}

void GuiElementFactory::CreateWindowBar(GuiWindowStyle style, GuiWindowElement &window)
{
    auto ptr = &window;
    const vec2 barPosition(0, window.GetScale().y + GUI_WINDOW_BAR_HEIGHT);

    auto horizontalLayout = CreateHorizontalLayout();
    horizontalLayout->SetScale(0.99f * vec2(window.GetScale().x, GUI_WINDOW_BAR_HEIGHT));
    horizontalLayout->SetPostion(barPosition);
    horizontalLayout->SetAlgin(Layout::Algin::RIGHT);

    auto barButton = CreateGuiButton([ptr](auto &) { ptr->CheckCollisionPoint(); });
    barButton->SetScale(vec2(window.GetScale().x, GUI_WINDOW_BAR_HEIGHT));
    barButton->SetPostion(barPosition);

    auto barTexture = CreateGuiTexture(theme_.windowBarTexture);
    if (barTexture)
    {
        barButton->SetBackgroundTexture(std::move(barTexture));
    }

    horizontalLayout->AddChild(std::move(barButton));

    if (style != GuiWindowStyle::WITHOUT_BUTTONS)
    {
        auto closeButton        = CreateGuiButton([this, ptr](auto &) { guiManager_.AddRemoveTask(ptr); });
        auto closeButtonTexture = CreateGuiTexture("GUI/close.png");
        if (closeButtonTexture)
        {
            closeButtonTexture->SetScale(.5f * vec2(GUI_WINDOW_BAR_HEIGHT));
            closeButton->SetScale(closeButtonTexture->GetScale());
            closeButton->SetBackgroundTexture(std::move(closeButtonTexture));
        }
        closeButton->SetZPosition(-1.f);
        horizontalLayout->AddChild(std::move(closeButton));
    }

    if (style == GuiWindowStyle::FULL)
    {
        DEBUG_LOG("GuiWindowElement::Style::FULL, minimalize, maxmalize buttons not implemented.");
    }

    window.SetBar(std::move(horizontalLayout));
}

bool GuiElementFactory::ReadGuiFile(const std::string &filename)
{
    return GuiElementReader(guiManager_, *this).Read(filename);
}
}  // namespace GameEngine
