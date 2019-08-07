#include "TextInput.h"
#include <Input/InputManager.h>
#include <Input/KeyCodeToCharConverter.h>
#include "Text/GuiTextElement.h"

namespace GameEngine
{
namespace
{
auto inputType = Input::SingleCharType::SMALL;
}

TextInput::TextInput(Input::InputManager& inputManager, GuiTextElement& text)
    : inputManager_(inputManager)
    , text_(text)
{
    lshiftDownSub_ = inputManager_.SubscribeOnKeyDown(KeyCodes::LSHIFT, [&]() { inputType = Input::SingleCharType::BIG; });
    rshiftDownSub_ = inputManager_.SubscribeOnKeyDown(KeyCodes::RSHIFT, [&]() { inputType = Input::SingleCharType::BIG; });
    lshiftUpSub_ = inputManager_.SubscribeOnKeyUp(KeyCodes::LSHIFT, [&]() { inputType = Input::SingleCharType::SMALL; });
    rshiftUpSub_ = inputManager_.SubscribeOnKeyUp(KeyCodes::RSHIFT, [&]() { inputType = Input::SingleCharType::SMALL; });
    anyKeySub_ = inputManager_.SubscribeOnAnyKeyPress([this](KeyCodes::Type key) {
        switch (key)
        {
            case KeyCodes::SPACE:
                text_.Append(' ');
                break;

            case KeyCodes::BACKSPACE:
                text_.Pop();
                break;

            default:
            {
                auto character = Input::KeyCodeToCharConverter::Convert(key, inputType);
                if (character)
                {
                    text_.Append(*character);
                }
            }
            break;
        }
    });
}
TextInput::~TextInput()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LSHIFT, lshiftDownSub_);
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::RSHIFT, rshiftDownSub_);

    inputManager_.UnsubscribeOnKeyUp(KeyCodes::LSHIFT, lshiftUpSub_);
    inputManager_.UnsubscribeOnKeyUp(KeyCodes::RSHIFT, rshiftUpSub_);

    inputManager_.UnsubscribeAnyKey(anyKeySub_);
}
}  // namespace GameEngine