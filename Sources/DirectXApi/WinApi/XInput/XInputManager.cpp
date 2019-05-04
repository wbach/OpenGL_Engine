#include <D3DX11.h>
#include <algorithm>
#include "Mutex.hpp"
#include "WinApiKeyConverter.h"
#include "XInputManager.h"

namespace DirectX
{
namespace
{
std::mutex keyEventMutex;

struct MouseState
{
    bool lButton_ = false;
    bool rButton_ = false;
    bool mButton_ = false;
};

MouseState mouseState;
}  // namespace

XInputManager::XInputManager(HWND windowHwnd, const vec2ui& windowSize)
    : windowHwnd_(windowHwnd)
    , halfWindowsSize_(windowSize.x / 2, windowSize.y / 2)
{
}

XInputManager::~XInputManager()
{
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

vec2i XInputManager::CalcualteMouseMove()
{
    auto mousePosition = GetMousePosition();

    vec2i dmove(0);
    dmove.x = halfWindowsSize_.x - mousePosition.x;
    dmove.y = halfWindowsSize_.y - mousePosition.y;
    SetCursorPosition(halfWindowsSize_.x, halfWindowsSize_.y);

    return dmove;
}

vec2i XInputManager::GetPixelMousePosition() 
{
    return vec2i();
}

vec2 XInputManager::GetMousePosition()
{
    POINT p;
    if (GetCursorPos(&p))
    {
        if (ScreenToClient(windowHwnd_, &p))
        {
            return vec2(p.x, p.y);
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

void XInputManager::ProcessKeysEvents()
{
    while (true)
    {
        auto e = GetEvent();

        if (!e)
            return;

        auto type  = e.value().first;
        auto value = e.value().second;

        auto keyCode = WinApiKeyConverter::Convert(value);

        if (type == WM_KEYDOWN)
        {
            UpdateMouseState(keyCode, true);
            for (const auto& subscriber : keyDownSubscribers_[keyCode])
            {
                subscriber();
            }
            for (const auto& keysSubscriber : keysSubscribers_)
            {
                keysSubscriber(keyCode);
            }
        }
        else if (type == WM_KEYUP)
        {
            UpdateMouseState(keyCode, false);

            for (const auto& subscriber : keyUpSubscribers_[keyCode])
            {
                subscriber();
            }
        }
    }
}

void XInputManager::AddKeyEvent(uint32 eventType, uint32 key)
{
    if (FindEvent(eventType, key))
        return;

    std::lock_guard<std::mutex> lk(keyEventMutex);
    keyEvents_.push_back({eventType, key});
}

void XInputManager::ShowCursor(bool show)
{
    ::ShowCursor(show ? TRUE : FALSE);
}

bool XInputManager::FindEvent(uint32 eventType, uint32 key)
{
    std::lock_guard<std::mutex> lk(keyEventMutex);

    auto iter = std::find_if(keyEvents_.begin(), keyEvents_.end(), [eventType, key](const KeyEvent& keyEvent) {
        return keyEvent.first == eventType and key == keyEvent.second;
    });

    return iter != keyEvents_.end();
}

std::optional<KeyEvent> XInputManager::GetEvent()
{
    std::lock_guard<std::mutex> lk(keyEventMutex);

    if (keyEvents_.empty())
        return std::optional<KeyEvent>();

    auto e = keyEvents_.front();
    keyEvents_.pop_front();
    return e;
}
void XInputManager::UpdateMouseState(uint32 keyCode, bool state)
{
    if (keyCode == KeyCodes::LMOUSE)
    {
        mouseState.lButton_ = state;
    }
    if (keyCode == KeyCodes::RMOUSE)
    {
        mouseState.rButton_ = state;
    }
    if (keyCode == KeyCodes::MOUSE_WHEEL)
    {
        mouseState.mButton_ = state;
    }
}
bool XInputManager::GetMouseState(uint32 keyCode)
{
    if (keyCode == KeyCodes::LMOUSE and mouseState.lButton_)
    {
        return true;
    }
    else if (keyCode == KeyCodes::RMOUSE and mouseState.rButton_)
    {
        return true;
    }
    else if (keyCode == KeyCodes::MOUSE_WHEEL and mouseState.mButton_)
    {
        return true;
    }
    return false;
}
}  // namespace DirectX
