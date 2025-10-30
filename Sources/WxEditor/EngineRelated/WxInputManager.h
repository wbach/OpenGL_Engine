#pragma once
#include <Input/InputManager.h>
#include <array>

class wxMouseEvent;

namespace WxEditor
{
class WxInputManager : public Input::InputManager
{
public:
    using KeyBuffer = std::array<bool, 1000>;
    using WrapPointerFunc = std::function<void (int, int)>;

    WxInputManager(const vec2i&, WrapPointerFunc);
    void OnMouseMove(wxMouseEvent&);

    bool GetKey(KeyCodes::Type);
    bool GetMouseKey(KeyCodes::Type);
    void SetReleativeMouseMode(bool);
    vec2i CalcualteMouseMove();
    vec2i GetPixelMousePosition();
    vec2 GetMousePosition();
    void SetKeyToBuffer(Input::KeyInteger, bool);
    void ClearKeyBuffer();
    void SetCursorPosition(int, int);
    void GetPressedKeys();
    void ShowCursor(bool);

    void SetWindowSize(const vec2i&);

protected:
    KeyCodes::Type ConvertCode(uint32) const;
    bool IsKeyUpEventType(uint32) const;
    bool IsKeyDownEventType(uint32) const;

private:
    vec2i windowSize;
    WrapPointerFunc wrapPointer;
    vec2i lastMousePosition;
    vec2i lastMouseMove;
    KeyBuffer keyBuffer;
    bool isRelativeMouseMode;
};
}  // namespace WxEditor
