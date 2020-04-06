#pragma once
#include "Input/InputManager.h"
#include "Mutex.hpp"
#include "Types.h"
#include "optional.hpp"

struct SDL_Window;

namespace OpenGLApi
{
typedef std::pair<uint32, uint32> KeyEvent;

class InputSDL : public Input::InputManager
{
public:
    InputSDL(SDL_Window* sdl_window);
    // Keyboard
    virtual bool GetKey(KeyCodes::Type i) override;

    // Mouse
    virtual bool GetMouseKey(KeyCodes::Type key) override;
    virtual vec2i CalcualteMouseMove() override;
    virtual vec2i GetPixelMousePosition() override;
    virtual vec2 GetMousePosition() override;

    // not use in SDL
    virtual void SetKeyToBuffer(int key, bool value) override;
    virtual void ClearKeyBuffer() override;
    virtual void SetCursorPosition(int x, int y) override;

    virtual void GetPressedKeys() override;
    virtual void ProcessKeysEvents() override;

    void AddKeyEvent(uint32 eventType, uint32 sdlKey);
    virtual void ShowCursor(bool) override;

private:
    bool FindEvent(uint32 eventType, uint32 sdlKey);
    wb::optional<KeyEvent> GetEvent();

private:
    std::mutex keyEventMutex_;

    std::list<KeyEvent> keyEvents_;
    SDL_Window* sdlWindow;
    vec2i windowsSize_;
    vec2i halfWindowsSize_;
};

}  // OpenGLApi
