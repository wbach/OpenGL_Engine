#pragma once
#include <Input/KeyCodes.h>
#include <Types.h>

#include <unordered_map>

namespace Input
{
class InputManager;
}

namespace GameEngine
{
namespace GUI
{
class Text;

class TextInput
{
public:
    TextInput(Input::InputManager&, Text&);
    ~TextInput();

private:
    Input::InputManager& inputManager_;
    Text& text_;
    uint32 lshiftDownSub_;
    uint32 lshiftUpSub_;
    uint32 rshiftDownSub_;
    uint32 rshiftUpSub_;
    uint32 anyKeySub_;
};
}  // namespace GUI
}  // namespace GameEngine
