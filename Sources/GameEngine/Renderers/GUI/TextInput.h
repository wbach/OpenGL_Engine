#pragma once
#include <Input/KeyCodes.h>
#include <unordered_map>
#include <Types.h>

namespace Input
{
class InputManager;
}

namespace GameEngine
{
class GuiTextElement;

class TextInput
{
public:
    TextInput(Input::InputManager&, GuiTextElement&);
    ~TextInput();

private:
    Input::InputManager& inputManager_;
    GuiTextElement& text_;
    uint32 lshiftDownSub_;
    uint32 lshiftUpSub_;
    uint32 rshiftDownSub_;
    uint32 rshiftUpSub_;
    uint32 anyKeySub_;
};
}  // namespace GameEngine
