#pragma once
#include <Windows.h>
#include <optional>
#include "Input/InputManager.h"
#include "Types.h"

namespace DirectX
{
typedef std::pair<uint32, uint32> KeyEvent;

class XInputManager : public Input::InputManager
{
public:
    XInputManager(HWND windowHwnd, const vec2ui& windowSize);
    ~XInputManager() override;
    // Keyboard
    virtual bool GetKey(KeyCodes::Type i) override;

    // Mouse
    virtual bool GetMouseKey(KeyCodes::Type key) override;
    virtual void SetReleativeMouseMode(bool) override;
    virtual vec2i CalcualteMouseMove() override;
    virtual vec2i GetPixelMousePosition() override;
    virtual vec2 GetMousePosition() override;

    virtual void SetKeyToBuffer(int key, bool value) override;
    virtual void ClearKeyBuffer() override;
    virtual void SetCursorPosition(int x, int y) override;

    virtual void GetPressedKeys() override;
    virtual void ProcessKeysEvents() override;

    void AddKeyEvent(uint32 eventType, uint32 sdlKey);
    virtual void ShowCursor(bool) override;

private:
    bool FindEvent(uint32 eventType, uint32 sdlKey);
    std::optional<KeyEvent> GetEvent();
    void UpdateMouseState(uint32 code, bool state);
    bool GetMouseState(uint32 code);

private:
    std::list<KeyEvent> keyEvents_;
    HWND windowHwnd_;
    vec2ui halfWindowsSize_;
    bool isRelativeMouseMode_;
    vec2i lastMouseMovmentPosition_;
};

}  // namespace DirectX
