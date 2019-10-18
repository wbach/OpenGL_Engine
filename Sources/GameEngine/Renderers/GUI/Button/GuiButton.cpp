#include "GuiButton.h"
#include <Logger/Log.h>
#include <algorithm>
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"

namespace GameEngine
{
const uint64 SHOW_ACTIVE_TIME = 100;

GuiElementTypes GuiButtonElement::type = GuiElementTypes::Button;

GuiButtonElement::GuiButtonElement(std::function<bool(GuiElement &)> isOnTop, Input::InputManager &inputManager,
                                   OnClick onClick, const vec2ui &windowSize)
    : GuiElement(type, windowSize)
    , inputManager_(inputManager)
    , onClick_(onClick)
    , text_{nullptr}
    , backgroundTexture_{nullptr}
    , onHoverTexture_{nullptr}
    , onActiveTextue_{nullptr}
    , isOnTop_{isOnTop}
{
    SubscribeInputAction();
}

GuiButtonElement::~GuiButtonElement()
{
    UnsubscribeInputAction();
}

void GuiButtonElement::Update()
{
    if (not IsShow())
    {
        return;
    }

    auto position = inputManager_.GetMousePosition();

    if (activeTimer_.GetTimeMiliSeconds() < SHOW_ACTIVE_TIME)
    {
        if (backgroundTexture_)
            backgroundTexture_->Hide();
        if (onHoverTexture_)
            onHoverTexture_->Hide();
        if (onActiveTextue_)
            onActiveTextue_->Show();
        if (text_)
            text_->SetColor(onActiveTextColor_);
        return;
    }

    if (IsCollision(position) and isOnTop_(*this))
    {
        if (backgroundTexture_)
            backgroundTexture_->Hide();
        if (onActiveTextue_)
            onActiveTextue_->Hide();
        if (onHoverTexture_)
            onHoverTexture_->Show();
        if (text_)
            text_->SetColor(onHoverTextColor_);
    }
    else
    {
        if (onHoverTexture_)
            onHoverTexture_->Hide();
        if (onActiveTextue_)
            onActiveTextue_->Hide();
        if (backgroundTexture_)
            backgroundTexture_->Show();
        if (text_)
            text_->SetColor(backgroundTextColor_);
    }
}

GuiElement *GuiButtonElement::GetCollisonElement(const vec2 &mousePosition)
{
    return IsCollision(mousePosition) ? this : nullptr;
}

void GuiButtonElement::SetText(std::unique_ptr<GuiTextElement> text)
{
    if (text_)
    {
        RemoveChild(text_->GetId());
    }

    text_ = text.get();
    text_->SetPostion(position_);
    AddChild(std::move(text));
    backgroundTextColor_ = text_->GetColor();
    onHoverTextColor_    = text_->GetColor();
    onActiveTextColor_   = text_->GetColor();
}

void GuiButtonElement::SetTexture(std::unique_ptr<GuiTextureElement> &newTexture, GuiTextureElement *&texture)
{
    newTexture->SetScale(scale_);
    newTexture->SetPostion(position_);

    if (texture)
    {
        RemoveChild(texture->GetId());
    }

    texture = newTexture.get();
    AddChild(std::move(newTexture));
}

void GuiButtonElement::SetBackgroundTexture(std::unique_ptr<GuiTextureElement> texture)
{
    SetTexture(texture, backgroundTexture_);
}

void GuiButtonElement::SetOnHoverTexture(std::unique_ptr<GuiTextureElement> texture)
{
    SetTexture(texture, onHoverTexture_);
}

void GuiButtonElement::SetOnActiveTexture(std::unique_ptr<GuiTextureElement> texture)
{
    SetTexture(texture, onActiveTextue_);
}

void GuiButtonElement::ResetBackgroundTexture()
{
    backgroundTexture_ = nullptr;
}

void GuiButtonElement::ResetOnHoverTexture()
{
    onHoverTexture_ = nullptr;
}

void GuiButtonElement::ResetOnActiveTexture()
{
    onActiveTextue_ = nullptr;
}

void GuiButtonElement::SetHoverTextColor(const vec3 &color)
{
    onHoverTextColor_ = color;
}

void GuiButtonElement::SetActiveTextColor(const vec3 &color)
{
    onActiveTextColor_ = color;
}

GuiTextElement *GuiButtonElement::GetText() const
{
    return text_;
}

GuiTextureElement *GuiButtonElement::GetBackgroundTexture() const
{
    return backgroundTexture_;
}

GuiTextureElement *GuiButtonElement::GetOnHoverTexture() const
{
    return onHoverTexture_;
}

GuiTextureElement *GuiButtonElement::GetOnActiveTexture() const
{
    return onActiveTextue_;
}

void GuiButtonElement::SetActionName(const std::string &actionName)
{
    actionName_ = actionName;
}

const std::string &GuiButtonElement::GetActionName() const
{
    return actionName_;
}

void GuiButtonElement::SubscribeInputAction()
{
    if (not subscribtion_)
    {
        subscribtion_ = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() {
            if (IsShow() and isOnTop_(*this))
            {
                auto position = inputManager_.GetMousePosition();
                if (IsCollision(position))
                {
                    onClick_(*this);
                    activeTimer_.Reset();
                }
            }
        });
    }
}

void GuiButtonElement::UnsubscribeInputAction()
{
    if (subscribtion_)
    {
        inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, *subscribtion_);
        subscribtion_.reset();
    }
    else
    {
        ERROR_LOG("Subscribtion not exist.");
    }
}

}  // namespace GameEngine
