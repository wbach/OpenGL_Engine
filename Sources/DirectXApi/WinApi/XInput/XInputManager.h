#pragma once
#include "Input/InputManager.h"
#include "Mutex.hpp"
#include "Types.h"
#include <optional>

namespace DirectX
{
typedef std::pair<uint32, uint32> KeyEvent;

class XInputManager : public Input::InputManager
{
public:
    XInputManager();
    ~XInputManager() override;
    // Keyboard
    virtual bool GetKey(KeyCodes::Type i) override;

    // Mouse
    virtual bool GetMouseKey(KeyCodes::Type key) override;
    virtual vec2 CalcualteMouseMove() override;
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

private:
    std::mutex keyEventMutex_;
    std::list<KeyEvent> keyEvents_;
};

}  // namespace DirectX
