#include "GuiButton.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"

namespace GameEngine
{
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
    inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [&]() {
        auto position = inputManager_.GetMousePosition();
        if (IsCollision(position))
        {
            onClick_();
        }
    });
}

void GuiButtonElement::Update()
{
    auto position = inputManager_.GetMousePosition();

    if (IsCollision(position))
    {
        if (backgroundTexture_)
            backgroundTexture_->Hide();
        if (onActiveTextue_)
            onActiveTextue_->Hide();
        if (onHoverTexture_)
            onHoverTexture_->Show();
    }
    else
    {
        if (backgroundTexture_)
            backgroundTexture_->Show();
        if (onActiveTextue_)
            onActiveTextue_->Hide();
        if (onHoverTexture_)
            onHoverTexture_->Hide();
    }
}

void GuiButtonElement::SetText(GuiTextElement *text)
{
    text_ = text;
}

void GuiButtonElement::SetBackgroundTexture(GuiTextureElement *texture)
{
    backgroundTexture_ = texture;
}

void GuiButtonElement::SetOnHoverTexture(GuiTextureElement *texture)
{
    onHoverTexture_ = texture;
}

void GuiButtonElement::SetOnActiveTexture(GuiTextureElement *texture)
{
    onActiveTextue_ = texture;
}

}  // namespace GameEngine
