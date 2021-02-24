#include "XInputManager.h"

#include <D3DX11.h>
#include <Logger/Log.h>

#include <algorithm>

#include "Mutex.hpp"
#include "WinApiKeyConverter.h"

namespace DirectX
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
    DEBUG_LOG("");
}
bool XInputManager::GetKey(KeyCodes::Type key)
{
    for (auto k : keyBuffer)
    {
        if (k == key)
            return true;
    }
    return false;
}
bool XInputManager::GetMouseKey(KeyCodes::Type key)
{
    return GetMouseState(key);
}
void XInputManager::SetReleativeMouseMode(bool v)
{
    isRelativeMouseMode_ = v;
    this->ShowCursor(not v);
}
vec2i XInputManager::CalcualteMouseMove()
{
    if (isRelativeMouseMode_)
    {
        auto mousePosition = GetPixelMousePosition();
        SetCursorPosition(halfWindowsSize_.x, halfWindowsSize_.y);
        return vec2i(mousePosition.x - halfWindowsSize_.x, mousePosition.y - halfWindowsSize_.y);
    }

    auto currentMousePosition = GetPixelMousePosition();
    vec2i result(currentMousePosition.x - lastMouseMovmentPosition_.x,
                 currentMousePosition.y - lastMouseMovmentPosition_.y);
    lastMouseMovmentPosition_ = currentMousePosition;
    return result;
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
void XInputManager::SetKeyToBuffer(int key, bool value)
{
}
void XInputManager::ClearKeyBuffer()
{
    keyBuffer.clear();
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
    ClearKeyBuffer();

    char keysArray[256];
    ZeroMemory(keysArray, 256);
    ReadKeyboard(keysArray);

    for (const auto& p : WinApiKeyConverter::GetKeysMap().GetXY())
    {
        if (keysArray[p.second])
        {
            keyBuffer.insert(p.first);
        }
    }
}
void XInputManager::ShowCursor(bool show)
{
    if (cursorShowState_ != show)
    {
        ::ShowCursor(show ? TRUE : FALSE);
        cursorShowState_ = show;
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
}  // namespace DirectX
