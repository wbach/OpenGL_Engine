#include "GuiEditText.h"
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>

namespace GameEngine
{
GuiElementTypes GuiEditBoxElement::type = GuiElementTypes::EditBox;

GuiEditBoxElement::GuiEditBoxElement(GuiTextElement &text, Input::InputManager &inputManager, const vec2ui &windowSize)
    : GuiElement(type, windowSize)
    , inputManager_(inputManager)
    , text_(text)
    , inputMode_(false)
    , charType_(Input::SingleCharType::SMALL)
{
    inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() {
        auto position = inputManager_.GetMousePosition();

        if (IsCollision(position))
        {
            inputMode_ = not inputMode_;
        }
    });

    inputManager_.SubscribeOnKeyDown(KeyCodes::ENTER, [this]() {
        if (inputMode_)
        {
            inputMode_ = false;
        }
    });

    inputManager_.SubscribeOnKeyDown(KeyCodes::BACKSPACE, [this]() {
        if (inputMode_)
        {
            text_.Pop();
        }
    });

    inputManager_.SubscribeOnAnyKeyPress([this](KeyCodes::Type key) {
        if (not inputMode_)
            return;

        auto character = Input::KeyCodeToCharConverter::Convert(key, charType_);
        if (character)
        {
            text_.Append(*character);
        }
    });
}
void GuiEditBoxElement::Update()
{
}
void GuiEditBoxElement::SetRect(const Rect &rect)
{
    text_.SetRect(rect);
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
