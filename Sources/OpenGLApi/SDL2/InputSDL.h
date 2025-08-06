#pragma once
#include "Input/InputManager.h"
#include "Mutex.hpp"
#include "Types.h"
#include "optional.hpp"

struct SDL_Window;

namespace OpenGLApi
{
class InputSDL : public Input::InputManager
{
public:
    InputSDL(SDL_Window*);
    bool GetKey(KeyCodes::Type i) override;
    bool GetMouseKey(KeyCodes::Type key) override;
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
    std::mutex keyBufferMutex_;
    std::mutex keyEventMutex_;
    SDL_Window* sdlWindow_;
    vec2i windowsSize_;
    bool isRelativeMouseMode;
    vec2i lastMouseMovmentPosition_;
};

}  // OpenGLApi
