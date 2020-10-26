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
    virtual bool GetKey(KeyCodes::Type i) override;
    virtual bool GetMouseKey(KeyCodes::Type key) override;
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
    std::mutex keyBufferMutex_;
    std::mutex keyEventMutex_;
    SDL_Window* sdlWindow_;
    vec2i windowsSize_;
    vec2i halfWindowsSize_;
    bool isRelativeMouseMode;
    vec2i lastMouseMovmentPosition_;
};

}  // OpenGLApi
