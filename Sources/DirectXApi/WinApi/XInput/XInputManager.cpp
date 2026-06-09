#include "XInputManager.h"

#include <Logger/Log.h>

#include <magic_enum/magic_enum.hpp>
#include "WinApiKeyConverter.h"

namespace GraphicsApi::Dx11
{
XInputManager::XInputManager(HWND windowHwnd, const vec2ui& windowSize)
    : windowHwnd_(windowHwnd)
    , windowSize_(windowSize)
    , halfWindowsSize_(windowSize.x / 2, windowSize.y / 2)
    , isRelativeMouseMode_(false)
    , lastMouseMovmentPosition_(GetPixelMousePosition())
    , cursorShowState_(true)
{
    ::ShowCursor(TRUE);
}
XInputManager::~XInputManager()
{
}
bool XInputManager::GetKey(KeyCodes::Type key)
{
    uint32 vk = WinApiKeyConverter::Convert(key);
    if (vk >= 256)
        return false;

    return (keyStates_[vk] & 0x80) != 0;
}
bool XInputManager::GetMouseKey(KeyCodes::Type key)
{
    uint32 vk = WinApiKeyConverter::Convert(key);
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}
void XInputManager::SetReleativeMouseMode(bool v)
{
    isRelativeMouseMode_ = v;
    this->ShowCursor(not v);
}
vec2i XInputManager::CalcualteMouseMove()
{
    auto currentMousePosition = GetPixelMousePosition();
    vec2i delta;

    if (isRelativeMouseMode_)
    {
        delta.x = currentMousePosition.x - halfWindowsSize_.x;
        delta.y = currentMousePosition.y - halfWindowsSize_.y;

        if (delta.x != 0 || delta.y != 0)
        {
            SetCursorPosition(halfWindowsSize_.x, halfWindowsSize_.y);
        }
    }
    else
    {
        delta.x                   = currentMousePosition.x - lastMouseMovmentPosition_.x;
        delta.y                   = currentMousePosition.y - lastMouseMovmentPosition_.y;
        lastMouseMovmentPosition_ = currentMousePosition;
    }

    return delta;
}
vec2i XInputManager::GetPixelMousePosition()
{
    POINT p;
    if (GetCursorPos(&p))
    {
        if (ScreenToClient(windowHwnd_, &p))
        {
            return vec2i(p.x, p.y);
        }
    }
    return vec2i(0);
}
vec2 XInputManager::GetMousePosition()
{
    POINT p;
    if (GetCursorPos(&p))
    {
        if (ScreenToClient(windowHwnd_, &p))
        {
            vec2 out;
            out.x = 2.f * (static_cast<float>(p.x) / static_cast<float>(windowSize_.x)) - 1.f;
            out.y = 2.f * (static_cast<float>(p.y) / static_cast<float>(windowSize_.y)) - 1.f;
            out.y *= -1.f;
            return out;
        }
    }
    return vec2(0);
}
void XInputManager::ClearKeyBuffer()
{
    memset(keyStates_, 0, sizeof(keyStates_));
}
void XInputManager::SetCursorPosition(int x, int y)
{
    POINT pt;
    pt.x = x;
    pt.y = y;
    ClientToScreen(windowHwnd_, &pt);
    SetCursorPos(pt.x, pt.y);
}
void ReadKeyboard(char* keys)
{
    for (int x = 0; x < 256; x++)
        keys[x] = (char)(GetAsyncKeyState(x) >> 8);
}
void XInputManager::GetPressedKeys()
{
    if (not GetKeyboardState(keyStates_))
    {
        memset(keyStates_, 0, sizeof(keyStates_));
    }
    keyStates_[VK_LSHIFT] = (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ? 0x80 : 0;
    keyStates_[VK_RSHIFT] = (GetAsyncKeyState(VK_RSHIFT) & 0x8000) ? 0x80 : 0;
}
void XInputManager::ShowCursor(bool show)
{
    isCursorVisible = show;

    if (show)
    {
        while (::ShowCursor(TRUE) < 0)
            ;
    }
    else
    {
        while (::ShowCursor(FALSE) >= 0)
            ;
    }
}
KeyCodes::Type XInputManager::ConvertCode(uint32 value) const
{
    return WinApiKeyConverter::Convert(value);
}
bool XInputManager::IsKeyUpEventType(uint32 type) const
{
    return (type == WM_KEYUP);
}
bool XInputManager::IsKeyDownEventType(uint32 type) const
{
    return (type == WM_KEYDOWN);
}
}  // namespace GraphicsApi::Dx11
