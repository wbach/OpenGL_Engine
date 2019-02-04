#include "XInputManager.h"

namespace DirectX
{
XInputManager::XInputManager()
{
}

XInputManager::~XInputManager()
{
}

bool XInputManager::GetKey(KeyCodes::Type i)
{
    return false;
}

bool XInputManager::GetMouseKey(KeyCodes::Type key)
{
    return false;
}

vec2 XInputManager::CalcualteMouseMove()
{
    return vec2();
}

vec2 XInputManager::GetMousePosition()
{
    return vec2();
}

void XInputManager::SetKeyToBuffer(int key, bool value)
{
}

void XInputManager::ClearKeyBuffer()
{
}

void XInputManager::SetCursorPosition(int x, int y)
{
}

void XInputManager::GetPressedKeys()
{
}

void XInputManager::ProcessKeysEvents()
{
}

void XInputManager::AddKeyEvent(uint32 eventType, uint32 sdlKey)
{
}

void XInputManager::ShowCursor(bool)
{
}

bool XInputManager::FindEvent(uint32 eventType, uint32 sdlKey)
{
    return false;
}

std::optional<KeyEvent> XInputManager::GetEvent()
{
    return std::optional<KeyEvent>();
}
}  // namespace DirectX
