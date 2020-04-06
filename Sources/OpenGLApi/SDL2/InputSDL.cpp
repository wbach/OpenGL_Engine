#include "InputSDL.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>

#include "Logger/Log.h"
#include "SdlKeyConverter.h"

namespace OpenGLApi
{
InputSDL::InputSDL(SDL_Window* sdl_window)
    : sdlWindow(sdl_window)
{
    SDL_GetWindowSize(sdlWindow, &windowsSize_.x, &windowsSize_.y);
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
    auto mousePosition = GetPixelMousePosition();
    SetCursorPosition(halfWindowsSize_.x, halfWindowsSize_.y);
    return vec2i(halfWindowsSize_.x - mousePosition.x, halfWindowsSize_.y - mousePosition.y);
}

bool InputSDL::GetMouseKey(KeyCodes::Type key)
{
    return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SdlKeyConverter::Convert(key));
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
    SDL_WarpMouseInWindow(sdlWindow, x, y);
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
}

void InputSDL::AddKeyEvent(uint32 eventType, uint32 sdlKey)
{
    if (FindEvent(eventType, sdlKey))
        return;

    std::lock_guard<std::mutex> lk(keyEventMutex_);
    keyEvents_.push_back({eventType, sdlKey});
}

void InputSDL::ShowCursor(bool is)
{
    SDL_ShowCursor(is ? SDL_ENABLE : SDL_DISABLE);
}

void InputSDL::ProcessKeysEvents()
{
    while (true)
    {
        auto e = GetEvent();

        if (!e)
            return;

        auto type  = e.value().first;
        auto value = e.value().second;

        auto keyCode = SdlKeyConverter::Convert(value);

        if (type == SDL_KEYDOWN)
        {
            ExecuteOnKeyDown(keyCode);
            ExecuteAnyKey(keyCode);
        }
        else if (type == SDL_KEYUP)
        {
            ExecuteOnKeyUp(keyCode);
        }
        Unquque();
    }
}

bool InputSDL::FindEvent(uint32 eventType, uint32 sdlKey)
{
    std::lock_guard<std::mutex> lk(keyEventMutex_);

    for (const auto& e : keyEvents_)
    {
        if (e.second == sdlKey && e.first == eventType)
            return true;
    }
    return false;
}

wb::optional<KeyEvent> InputSDL::GetEvent()
{
    std::lock_guard<std::mutex> lk(keyEventMutex_);

    if (keyEvents_.empty())
        return wb::optional<KeyEvent>();

    auto e = keyEvents_.front();
    keyEvents_.pop_front();
    return e;
}

void InputSDL::SetKeyToBuffer(int, bool)
{
}

void InputSDL::ClearKeyBuffer()
{
    keyBuffer.clear();
}
}  // namespace OpenGLApi
