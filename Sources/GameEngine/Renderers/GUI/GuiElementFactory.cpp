#include "GuiElementFactory.h"
#include <algorithm>
#include "Button/GuiButton.h"
#include "EditText/GuiEditText.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GuiElementsDef.h"
#include "GuiManager.h"
#include "Layout/HorizontalLayout.h"
#include "Layout/VerticalLayout.h"
#include "Text/GuiTextElement.h"
#include "Text/GuiTextFactory.h"
#include "Texutre/GuiTextureElement.h"
#include "GuiElementReader.h"
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

    if (not backgorund.empty() and backgorund != "none")
    {
        auto backgroundGuiTexture = MakeGuiTexture(backgorund);
        if (backgroundGuiTexture)
        {
            backgroundGuiTexture->SetZPosition(0.5f);
            backgroundGuiTexture->SetScale(guiWindow->GetScale());
            guiWindow->AddMember(std::move(backgroundGuiTexture));
        }
    }

    CreateWindowBar(*guiWindow);
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

void GuiElementFactory::CreateWindowBar(GuiWindowElement &window)
{
    auto ptr              = &window;
    const float barHeight = 0.04f;
    const vec2 barPosition(0, window.GetScale().y + barHeight);

    auto horizontalLayout = CreateHorizontalLayout();
    horizontalLayout->SetScale(0.99f * vec2(window.GetScale().x, barHeight));
    horizontalLayout->SetPostion(barPosition);
    horizontalLayout->SetAlgin(Layout::Algin::RIGHT);

    auto barButton  = CreateGuiButton([ptr](auto &) { ptr->CheckCollisionPoint(); });
    auto barTexture = CreateGuiTexture(theme_.windowBarTexture);
    barButton->SetScale(vec2(window.GetScale().x, barHeight));
    barButton->SetPostion(barPosition);
    barButton->SetBackgroundTexture(std::move(barTexture));
    window.AddMember(std::move(barButton));

    {
        auto closeButton        = CreateGuiButton([ptr](auto &) { ptr->MarkToRemove(); });
        auto closeButtonTexture = CreateGuiTexture("GUI/close.png");
        closeButtonTexture->SetScale(.5f * vec2(barHeight));
        closeButton->SetScale(closeButtonTexture->GetScale());
        closeButton->SetBackgroundTexture(std::move(closeButtonTexture));
        closeButton->SetZPosition(-1.f);
        horizontalLayout->AddChild(std::move(closeButton));
    }

    window.AddMember(std::move(horizontalLayout));
}



bool GuiElementFactory::ReadGuiFile(const std::string &filename)
{
    return GuiElementReader(guiManager_, *this).Read(filename);
}
}  // namespace GameEngine
