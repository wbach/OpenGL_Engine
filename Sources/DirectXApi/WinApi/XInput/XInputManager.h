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

    virtual bool GetKey(KeyCodes::Type) override;
    virtual bool GetMouseKey(KeyCodes::Type) override;
    virtual void SetReleativeMouseMode(bool) override;
    virtual vec2i CalcualteMouseMove() override;
    virtual vec2i GetPixelMousePosition() override;
    virtual vec2 GetMousePosition() override;
    virtual void SetKeyToBuffer(int key, bool value) override;
    virtual void ClearKeyBuffer() override;
    virtual void SetCursorPosition(int x, int y) override;
    virtual void GetPressedKeys() override;
    virtual void ShowCursor(bool) override;

protected:
    virtual KeyCodes::Type ConvertCode(uint32) const override;
    virtual bool IsKeyUpEventType(uint32) const override;
    virtual bool IsKeyDownEventType(uint32) const override;

private:
    HWND windowHwnd_;
    vec2ui windowSize_;
    vec2ui halfWindowsSize_;
    bool isRelativeMouseMode_;
    vec2i lastMouseMovmentPosition_;
};

}  // namespace DirectX
