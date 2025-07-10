#pragma once
#include <Input/InputManager.h>

namespace WxEditor
{
class WxInputManager : public Input::InputManager
{
public:
    bool GetKey(KeyCodes::Type);
    bool GetMouseKey(KeyCodes::Type);
    void SetReleativeMouseMode(bool);
    vec2i CalcualteMouseMove();
    vec2i GetPixelMousePosition();
    vec2 GetMousePosition();
    void SetKeyToBuffer(int, bool);
    void ClearKeyBuffer();
    void SetCursorPosition(int, int);
    void GetPressedKeys();
    void ShowCursor(bool);

protected:
    KeyCodes::Type ConvertCode(uint32 value) const;
    bool IsKeyUpEventType(uint32 x) const;
    bool IsKeyDownEventType(uint32 x) const;
};
}  // namespace WxEditor
