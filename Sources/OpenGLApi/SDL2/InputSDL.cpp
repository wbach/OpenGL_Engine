#include "InputSDL.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>

#include "Logger/Log.h"
#include "SdlKeyConverter.h"

namespace OpenGLApi
{
InputSDL::InputSDL(SDL_Window* sdlWindow)
    : sdlWindow_(sdlWindow)
    , isRelativeMouseMode(false)
    , lastMouseMovmentPosition_(GetPixelMousePosition())
{
    SDL_GetWindowSize(sdlWindow_, &windowsSize_.x, &windowsSize_.y);
    halfWindowsSize_ = vec2i{windowsSize_.x / 2, windowsSize_.y / 2};
}
bool InputSDL::GetKey(KeyCodes::Type key)
{
    for (auto k : keyBuffer)
    {
        if (k == key)
            return true;
    }
    return false;
}
vec2i InputSDL::CalcualteMouseMove()
{
    vec2i result;
    if (isRelativeMouseMode)
    {
        SDL_GetRelativeMouseState(&result.x, &result.y);
    }
    else
    {
        auto currentMousePosition = GetPixelMousePosition();
        result.x                  = lastMouseMovmentPosition_.x - currentMousePosition.x;
        result.y                  = lastMouseMovmentPosition_.y - currentMousePosition.y;
        lastMouseMovmentPosition_ = currentMousePosition;
    }
    return result;
}
bool InputSDL::GetMouseKey(KeyCodes::Type key)
{
    return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SdlKeyConverter::Convert(key));
}
void InputSDL::SetReleativeMouseMode(bool v)
{
    isRelativeMouseMode = v;
    SDL_SetRelativeMouseMode(v ? SDL_TRUE : SDL_FALSE);
}
vec2i InputSDL::GetPixelMousePosition()
{
    vec2i result;
    SDL_GetMouseState(&result.x, &result.y);
    return result;
}
vec2 InputSDL::GetMousePosition()
{
    auto mousePosition = GetPixelMousePosition();

    vec2 out;
    out.x = 2.f * (static_cast<float>(mousePosition.x) / static_cast<float>(windowsSize_.x)) - 1.f;
    out.y = 2.f * (static_cast<float>(mousePosition.y) / static_cast<float>(windowsSize_.y)) - 1.f;
    out.y *= -1.f;
    return out;
}
void InputSDL::SetCursorPosition(int x, int y)
{
    SDL_WarpMouseInWindow(sdlWindow_, x, y);
}
void InputSDL::GetPressedKeys()
{
    keyBuffer.clear();
    int32 arraySize;
    const Uint8* state = SDL_GetKeyboardState(&arraySize);

    for (const auto& p : SdlKeyConverter::keys.GetXY())
    {
        if (state[p.second])
        {
            keyBuffer.insert(p.first);
        }
    }

    if (GetMouseKey(KeyCodes::LMOUSE))
    {
        keyBuffer.insert(KeyCodes::LMOUSE);
    }
    if (GetMouseKey(KeyCodes::RMOUSE))
    {
        keyBuffer.insert(KeyCodes::RMOUSE);
    }
}
void InputSDL::ShowCursor(bool is)
{
    SDL_ShowCursor(is ? SDL_ENABLE : SDL_DISABLE);
}
KeyCodes::Type InputSDL::ConvertCode(uint32 value) const
{
    return SdlKeyConverter::Convert(value);
}
bool InputSDL::IsKeyUpEventType(uint32 type) const
{
    return type == SDL_KEYUP;
}
bool InputSDL::IsKeyDownEventType(uint32 type) const
{
    return type == SDL_KEYDOWN;
}
void InputSDL::SetKeyToBuffer(int, bool)
{
}
void InputSDL::ClearKeyBuffer()
{
    keyBuffer.clear();
}
}  // namespace OpenGLApi
