#include "GuiEditText.h"
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include "GameEngine/Renderers/GUI/TextInput.h"
#include "GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"

namespace GameEngine
{
GuiElementTypes GuiEditBoxElement::type = GuiElementTypes::EditBox;

GuiEditBoxElement::GuiEditBoxElement(GuiTextElement &text, Input::InputManager &inputManager, const vec2ui &windowSize)
    : GuiElement(type, windowSize)
    , inputManager_(inputManager)
    , text_(text)
    , inputMode_(false)

{
    lmouseSubscribtrion_ = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() {
        auto position = inputManager_.GetMousePosition();

        if (IsCollision(position))
        {
            if (textInput_)
            {
                textInput_.reset();
            }
            else
            {
                textInput_ = std::make_unique<TextInput>(inputManager_, text_);
            }
        }
    });

    entersubscribion_ = inputManager_.SubscribeOnKeyDown(KeyCodes::ENTER, [this]() {
        if (inputMode_)
        {
            textInput_.reset();
        }
    });
}
GuiEditBoxElement::~GuiEditBoxElement()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, lmouseSubscribtrion_);
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::ENTER, entersubscribion_);
}
void GuiEditBoxElement::Update()
{
}
void GuiEditBoxElement::SetBackgroundTexture(GuiTextureElement *texture)
{
    backgroundTexture_ = texture;
}
void GuiEditBoxElement::SetRect(const Rect &rect)
{
    text_.SetRect(rect);
    if (backgroundTexture_) backgroundTexture_->SetRect(rect);
    GuiElement::SetRect(rect);
}

void GuiEditBoxElement::SetSize(const vec2ui &size)
{
    text_.SetSize(size);
    GuiElement::SetSize(size);
}

void GuiEditBoxElement::SetScale(const vec2 &scale)
{
    text_.SetScale(scale);
    GuiElement::SetScale(scale);
}

void GuiEditBoxElement::SetPostion(const vec2 &position)
{
    text_.SetPostion(position);
    GuiElement::SetPostion(position);
}

void GuiEditBoxElement::SetPostion(const vec2ui &position)
{
    text_.SetPostion(position);
    GuiElement::SetPostion(position);
}

void GuiEditBoxElement::Rotate(float r)
{
    text_.Rotate(r);
    GuiElement::Rotate(r);
}

void GuiEditBoxElement::Show(bool b)
{
    text_.Show(b);
    GuiElement::Show(b);
}

void GuiEditBoxElement::Show()
{
    text_.Show();
    GuiElement::Show();
}

void GuiEditBoxElement::Hide()
{
    text_.Hide();
    GuiElement::Hide();
}
}  // namespace GameEngine
