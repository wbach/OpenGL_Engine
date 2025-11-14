#include "GuiButton.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"

namespace GameEngine
{
const uint64 SHOW_ACTIVE_TIME = 100;

GuiElementTypes GuiButtonElement::type = GuiElementTypes::Button;

GuiButtonElement::GuiButtonElement(std::function<bool(const GuiElement &)> isOnTop, Input::InputManager &inputManager,
                                   OnClick onClick)
    : GuiElement(type)
    , inputManager_(inputManager)
    , onClick_(onClick)
    , text_{nullptr}
    , backgroundTexture_{nullptr}
    , onHoverTexture_{nullptr}
    , onActiveTextue_{nullptr}
    , isOnTop_{isOnTop}
    , currentState_{State::Normal}
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

    auto state = GetCurrentState();

    if (currentState_ != state)
    {
        ApplyState(state);
        currentState_ = state;
    }
}

void GuiButtonElement::Show()
{
    GuiElement::Show();
    if (onHoverTexture_)
    {
        onHoverTexture_->Hide();
    }
    if (onActiveTextue_)
    {
        onActiveTextue_->Hide();
    }
}

void GuiButtonElement::Show(bool b)
{
    GuiElement::Show(b);
    if (onHoverTexture_)
    {
        onHoverTexture_->Hide();
    }
    if (onActiveTextue_)
    {
        onActiveTextue_->Hide();
    }
}

GuiElement *GuiButtonElement::GetCollisonElement(const vec2 &mousePosition)
{
    return IsCollision(mousePosition) ? this : nullptr;
}

void GuiButtonElement::ExecuteAction()
{
    if (onClick_)
        onClick_(*this);
}

void GuiButtonElement::Highlight()
{
    ApplyHoverState();
}

void GuiButtonElement::ToneDown()
{
    ApplyState(currentState_);
}

void GuiButtonElement::SetText(std::unique_ptr<GuiTextElement> text)
{
    if (text_)
    {
        RemoveChild(text_->GetId());
    }

    text_ = text.get();
    text_->SetZPosition(-0.2f);
    AddChild(std::move(text));
    backgroundTextColor_ = text_->GetColor();
    onHoverTextColor_    = text_->GetColor();
    onActiveTextColor_   = text_->GetColor();
}

void GuiButtonElement::SetTexture(std::unique_ptr<GuiTextureElement> &newTexture, GuiTextureElement *&texture)
{
    newTexture->SetZPosition(-0.1f);

    if (texture)
    {
        RemoveChild(texture->GetId());
    }

    texture = newTexture.get();
    AddChild(std::move(newTexture));
}

void GuiButtonElement::SetBackgroundTexture(std::unique_ptr<GuiTextureElement> texture)
{
    if (texture)
    {
        SetTexture(texture, backgroundTexture_);
    }
}

void GuiButtonElement::SetOnHoverTexture(std::unique_ptr<GuiTextureElement> texture)
{
    if (texture)
    {
        texture->Hide();
        SetTexture(texture, onHoverTexture_);
    }
}

void GuiButtonElement::SetOnActiveTexture(std::unique_ptr<GuiTextureElement> texture)
{
    if (texture)
    {
        texture->Hide();
        SetTexture(texture, onActiveTextue_);
    }
}

void GuiButtonElement::ResetBackgroundTexture()
{
    if (backgroundTexture_)
    {
        RemoveChild(backgroundTexture_->GetId());
    }
    backgroundTexture_ = nullptr;
}

void GuiButtonElement::ResetOnHoverTexture()
{
    if (onHoverTexture_)
    {
        RemoveChild(onHoverTexture_->GetId());
    }
    onHoverTexture_ = nullptr;
}

void GuiButtonElement::ResetOnActiveTexture()
{
    if (onActiveTextue_)
    {
        RemoveChild(onActiveTextue_->GetId());
    }
    onActiveTextue_ = nullptr;
}

void GuiButtonElement::SetHoverTextColor(const vec4 &color)
{
    onHoverTextColor_ = color;
}

void GuiButtonElement::SetActiveTextColor(const vec4 &color)
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
        subscribtion_ = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE,
                                                         [this]()
                                                         {
                                                             if (IsShow() and isOnTop_(*this))
                                                             {
                                                                 auto position = inputManager_.GetMousePosition();
                                                                 if (IsCollision(position))
                                                                 {
                                                                     if (onClick_)
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
        /* LOG TO FIX*/  LOG_ERROR << ("Subscribtion not exist.");
    }
}

GuiButtonElement::State GuiButtonElement::GetCurrentState() const
{
    if (activeTimer_.GetTimeMilliseconds() < SHOW_ACTIVE_TIME)
    {
        return State::Active;
    }

    auto position = inputManager_.GetMousePosition();

    if (IsCollision(position) and isOnTop_(*this))
    {
        return State::Hover;
    }

    return State::Normal;
}

void GuiButtonElement::ApplyState(State state)
{
    switch (state)
    {
        case State::Normal:
            ApplyNormalState();
            break;
        case State::Hover:
            ApplyHoverState();
            break;
        case State::Active:
            ApplyActiveState();
            break;
    }
}

void GuiButtonElement::ApplyNormalState()
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

void GuiButtonElement::ApplyHoverState()
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

void GuiButtonElement::ApplyActiveState()
{
    if (backgroundTexture_)
        backgroundTexture_->Hide();
    if (onHoverTexture_)
        onHoverTexture_->Hide();
    if (onActiveTextue_)
        onActiveTextue_->Show();
    if (text_)
        text_->SetColor(onActiveTextColor_);
}

}  // namespace GameEngine
