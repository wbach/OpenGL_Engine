#include "GuiButton.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"

namespace GameEngine
{
const uint64 SHOW_ACTIVE_TIME = 100;

GuiElementTypes GuiButtonElement::type = GuiElementTypes::Button;

GuiButtonElement::GuiButtonElement(Input::InputManager &inputManager, OnClick onClick, const vec2ui &windowSize)
    : GuiElement(type, windowSize)
    , inputManager_(inputManager)
    , onClick_(onClick)
    , text_{nullptr}
    , backgroundTexture_{nullptr}
    , onHoverTexture_{nullptr}
    , onActiveTextue_{nullptr}
{
    auto &timer = activeTimer_;
    inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [&]() {
        auto position = inputManager_.GetMousePosition();
        if (IsCollision(position))
        {
            onClick_();
            timer.Reset();
        }
    });
}

void GuiButtonElement::Update()
{
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

    if (IsCollision(position))
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

void GuiButtonElement::SetText(GuiTextElement *text)
{
    text_ = text;
    text_->SetPostion(position_);

    backgroundTextColor_ = text_->GetColor();
    onHoverTextColor_    = text_->GetColor();
    onActiveTextColor_   = text_->GetColor();
}

void GuiButtonElement::SetBackgroundTexture(GuiTextureElement *texture)
{
    backgroundTexture_ = texture;
    backgroundTexture_->SetScale(scale_);
    backgroundTexture_->SetPostion(position_);
}

void GuiButtonElement::SetOnHoverTexture(GuiTextureElement *texture)
{
    onHoverTexture_ = texture;
    onHoverTexture_->SetScale(scale_);
    onHoverTexture_->SetPostion(position_);
}

void GuiButtonElement::SetOnActiveTexture(GuiTextureElement *texture)
{
    onActiveTextue_ = texture;
    onActiveTextue_->SetScale(scale_);
    onActiveTextue_->SetPostion(position_);
}

void GuiButtonElement::SetHoverTextColor(const vec3 &color)
{
    onHoverTextColor_ = color;
}

void GuiButtonElement::SetActiveTextColor(const vec3 &color)
{
    onActiveTextColor_ = color;
}

}  // namespace GameEngine
