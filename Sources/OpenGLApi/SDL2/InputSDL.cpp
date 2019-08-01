#include "InputSDL.h"
#include "SdlKeyConverter.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>

#include "Logger/Log.h"

namespace OpenGLApi
{
InputSDL::InputSDL(SDL_Window* sdl_window)
    : sdlWindow(sdl_window)
{
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
    int w = 320;
    int h = 240;

    SDL_GetWindowSize(sdlWindow, &w, &h);

    int tmp_x, tmp_y;
    SDL_GetMouseState(&tmp_x, &tmp_y);

    vec2i dmove(0);
    dmove.x = w / 2 - tmp_x;
    dmove.y = h / 2 - tmp_y;
    SDL_WarpMouseInWindow(sdlWindow, w / 2, h / 2);

    return dmove;
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
    int tmp_x, tmp_y;
    int w = 320;
    int h = 240;
    SDL_GetWindowSize(sdlWindow, &w, &h);
    SDL_GetMouseState(&tmp_x, &tmp_y);

    vec2 out;
    out.x = 2.f * (static_cast<float>(tmp_x) / static_cast<float>(w)) - 1.f;
    out.y = 2.f * (static_cast<float>(tmp_y) / static_cast<float>(h)) - 1.f;
    out.y *= -1.f;
    return out;
}

void InputSDL::SetCursorPosition(int, int)
{
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
            if (subscribers_.keyDownSubscribers_.count(keyCode) > 0)
            {
                auto subscribers = subscribers_.keyDownSubscribers_.at(keyCode);

                for (const auto& subscriber : subscribers)
                {
                    subscriber.second();
                }
            }

            auto subscribers = subscribers_.keysSubscribers_;
            for (const auto& keysSubscriber : subscribers_.keysSubscribers_)
            {
                keysSubscriber.second(keyCode);
            }
        }
        else if (type == SDL_KEYUP)
        {
            if (subscribers_.keyUpSubscribers_.count(keyCode) > 0)
            {
                auto subscribers = subscribers_.keyUpSubscribers_.at(keyCode);

                for (const auto& subscriber : subscribers)
                {
                    subscriber.second();
                }
            }
        }
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
