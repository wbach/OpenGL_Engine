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
    SubscribeInputAction();
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

void GuiButtonElement::SetRect(const Rect &rect)
{
    if (onHoverTexture_)
        onHoverTexture_->SetRect(rect);
    if (onActiveTextue_)
        onActiveTextue_->SetRect(rect);
    if (backgroundTexture_)
        backgroundTexture_->SetRect(rect);
    if (text_)
        text_->SetRect(rect);

    GuiElement::SetRect(rect);
}

void GuiButtonElement::SetSize(const vec2ui &size)
{
    if (onHoverTexture_)
        onHoverTexture_->SetSize(size);
    if (onActiveTextue_)
        onActiveTextue_->SetSize(size);
    if (backgroundTexture_)
        backgroundTexture_->SetSize(size);
    // if (text_)
    //    text_->SetSize(size);

    GuiElement::SetSize(size);
}

void GuiButtonElement::SetScale(const vec2 &scale)
{
    if (onHoverTexture_)
        onHoverTexture_->SetScale(scale);
    if (onActiveTextue_)
        onActiveTextue_->SetScale(scale);
    if (backgroundTexture_)
        backgroundTexture_->SetScale(scale);
    // if (text_)
    //    text_->SetScale(scale);

    GuiElement::SetScale(scale);
}

void GuiButtonElement::SetPostion(const vec2 &position)
{
    if (onHoverTexture_)
        onHoverTexture_->SetPostion(position);
    if (onActiveTextue_)
        onActiveTextue_->SetPostion(position);
    if (backgroundTexture_)
        backgroundTexture_->SetPostion(position);
    if (text_)
        text_->SetPostion(position);

    GuiElement::SetPostion(position);
}

void GuiButtonElement::SetPostion(const vec2ui &position)
{
    if (onHoverTexture_)
        onHoverTexture_->SetPostion(position);
    if (onActiveTextue_)
        onActiveTextue_->SetPostion(position);
    if (backgroundTexture_)
        backgroundTexture_->SetPostion(position);
    if (text_)
        text_->SetPostion(position);

    GuiElement::SetPostion(position);
}

void GuiButtonElement::Rotate(float r)
{
    if (onHoverTexture_)
        onHoverTexture_->Rotate(r);
    if (onActiveTextue_)
        onActiveTextue_->Rotate(r);
    if (backgroundTexture_)
        backgroundTexture_->Rotate(r);
    if (text_)
        text_->Rotate(r);

    GuiElement::Rotate(r);
}

void GuiButtonElement::Show(bool b)
{
    if (onHoverTexture_)
        onHoverTexture_->Show(b);
    if (onActiveTextue_)
        onActiveTextue_->Show(b);
    if (backgroundTexture_)
        backgroundTexture_->Show(b);
    if (text_)
        text_->Show(b);

    GuiElement::Show(b);

    //b ? SubscribeInputAction() : UnsubscribeInputAction();
}

void GuiButtonElement::Show()
{
    if (onHoverTexture_)
        onHoverTexture_->Show();
    if (onActiveTextue_)
        onActiveTextue_->Show();
    if (backgroundTexture_)
        backgroundTexture_->Show();
    if (text_)
        text_->Show();

    GuiElement::Show();

    //SubscribeInputAction();
}

void GuiButtonElement::Hide()
{
    if (onHoverTexture_)
        onHoverTexture_->Hide();
    if (onActiveTextue_)
        onActiveTextue_->Hide();
    if (backgroundTexture_)
        backgroundTexture_->Hide();
    if (text_)
        text_->Hide();

    GuiElement::Hide();

    //UnsubscribeInputAction();
}

void GuiButtonElement::execute(std::function<void(uint32)> function)
{
    if (onHoverTexture_)
        function(onHoverTexture_->GetId());
    if (onActiveTextue_)
        function(onActiveTextue_->GetId());
    if (backgroundTexture_)
        function(backgroundTexture_->GetId());
    if (text_)
        function(text_->GetId());
}

void GuiButtonElement::SubscribeInputAction()
{
    subscribtion_ = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [&]() {
        if (IsShow())
        {
            auto position = inputManager_.GetMousePosition();
            if (IsCollision(position))
            {
                onClick_();
                activeTimer_.Reset();
            }
        }
    });
}

void GuiButtonElement::UnsubscribeInputAction()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, subscribtion_);
}

}  // namespace GameEngine
