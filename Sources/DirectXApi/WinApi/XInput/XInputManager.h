#pragma once
#include <Windows.h>
#include <optional>
#include "Input/InputManager.h"
#include "Types.h"

namespace DirectX
{
class XInputManager : public Input::InputManager
{
public:
    XInputManager(HWND windowHwnd, const vec2ui& windowSize);
    ~XInputManager() override;

    bool GetKey(KeyCodes::Type) override;
    bool GetMouseKey(KeyCodes::Type) override;
    void SetReleativeMouseMode(bool) override;
    vec2i CalcualteMouseMove() override;
    vec2i GetPixelMousePosition() override;
    vec2 GetMousePosition() override;
    void ClearKeyBuffer() override;
    void SetCursorPosition(int x, int y) override;
    void GetPressedKeys() override;
    void ShowCursor(bool) override;

protected:
    KeyCodes::Type ConvertCode(uint32) const override;
    bool IsKeyUpEventType(uint32) const override;
    bool IsKeyDownEventType(uint32) const override;

private:
    HWND windowHwnd_;
    vec2ui windowSize_;
    vec2ui halfWindowsSize_;
    bool isRelativeMouseMode_;
    vec2i lastMouseMovmentPosition_;
    bool cursorShowState_;
};

}  // namespace DirectX
