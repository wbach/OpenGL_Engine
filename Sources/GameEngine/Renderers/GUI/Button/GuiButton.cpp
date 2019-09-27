#include "GuiButton.h"
#include <Logger/Log.h>
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"
namespace GameEngine
{
const uint64 SHOW_ACTIVE_TIME = 100;

GuiElementTypes GuiButtonElement::type = GuiElementTypes::Button;

GuiButtonElement::GuiButtonElement(std::function<bool(GuiElement &)> isOnTop, Input::InputManager &inputManager, OnClick onClick, const vec2ui &windowSize)
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

void GuiButtonElement::SetText(std::unique_ptr<GuiTextElement> text)
{
    if (text_)
    {
        text_->MarkToRemove();
    }

    text_ = std::move(text);

    text_->SetPostion(position_);

    backgroundTextColor_ = text_->GetColor();
    onHoverTextColor_    = text_->GetColor();
    onActiveTextColor_   = text_->GetColor();
}

void GuiButtonElement::SetTexture(std::unique_ptr<GuiTextureElement> &a, std::unique_ptr<GuiTextureElement> &b)
{
    if (b)
    {
        b->MarkToRemove();
    }

    a->SetScale(scale_);
    a->SetPostion(position_);
    b = std::move(a);
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
    backgroundTexture_.reset();
}

void GuiButtonElement::ResetOnHoverTexture()
{
    onHoverTexture_.reset();
}

void GuiButtonElement::ResetOnActiveTexture()
{
    onActiveTextue_.reset();
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

void GuiButtonElement::SetZPosition(float z)
{
    GuiElement::SetZPosition(z);

    if (onHoverTexture_)
        onHoverTexture_->SetZPositionOffset(GetZTotalValue());
    if (onActiveTextue_)
        onActiveTextue_->SetZPositionOffset(GetZTotalValue());
    if (backgroundTexture_)
        backgroundTexture_->SetZPositionOffset(GetZTotalValue());
    if (text_)
        text_->SetZPositionOffset(GetZTotalValue());
}

void GuiButtonElement::SetZPositionOffset(float offset)
{
    GuiElement::SetZPositionOffset(offset);

    if (onHoverTexture_)
        onHoverTexture_->SetZPositionOffset(GetZTotalValue());
    if (onActiveTextue_)
        onActiveTextue_->SetZPositionOffset(GetZTotalValue());
    if (backgroundTexture_)
        backgroundTexture_->SetZPositionOffset(GetZTotalValue());
    if (text_)
        text_->SetZPositionOffset(GetZTotalValue());
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

void GuiButtonElement::SetIsInternal(bool is)
{
    if (onHoverTexture_)
        onHoverTexture_->SetIsInternal(is);
    if (onActiveTextue_)
        onActiveTextue_->SetIsInternal(is);
    if (backgroundTexture_)
        backgroundTexture_->SetIsInternal(is);
    if (text_)
        text_->SetIsInternal(is);

    GuiElement::SetIsInternal(is);
}

GuiTextElement *GuiButtonElement::GetText() const
{
    return text_.get();
}

GuiTextureElement *GuiButtonElement::GetBackgroundTexture() const
{
    return backgroundTexture_.get();
}

GuiTextureElement *GuiButtonElement::GetOnHoverTexture() const
{
    return onHoverTexture_.get();
}

GuiTextureElement *GuiButtonElement::GetOnActiveTexture() const
{
    return onActiveTextue_.get();
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
        subscribtion_ = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [&]() {
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
}

}  // namespace GameEngine
