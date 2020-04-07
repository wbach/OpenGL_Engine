#include "DummyInputManager.h"

#include <iostream>

namespace GameEngine
{
DummyInputManager::DummyInputManager()
    : run(true)
{
    upThread = std::thread(&DummyInputManager::Update, this);
}
DummyInputManager::~DummyInputManager()
{
    run.store(false);
    upThread.join();
}
bool GameEngine::DummyInputManager::GetKey(KeyCodes::Type key)
{
    return IsKey(key);
}
bool GameEngine::DummyInputManager::GetMouseKey(KeyCodes::Type)
{
    return false;
}
void DummyInputManager::SetReleativeMouseMode(bool)
{
}
vec2i GameEngine::DummyInputManager::CalcualteMouseMove()
{
    return vec2i();
}
vec2i DummyInputManager::GetPixelMousePosition()
{
    return vec2i();
}
vec2 GameEngine::DummyInputManager::GetMousePosition()
{
    return vec2();
}
void GameEngine::DummyInputManager::SetCursorPosition(int, int)
{
}
void GameEngine::DummyInputManager::SetKeyToBuffer(int, bool)
{
}
void GameEngine::DummyInputManager::ClearKeyBuffer()
{
}
void GameEngine::DummyInputManager::GetPressedKeys()
{
}
void GameEngine::DummyInputManager::ShowCursor(bool)
{
}
KeyCodes::Type DummyInputManager::ConvertCode(uint32) const
{
    return KeyCodes::Type();
}
bool DummyInputManager::IsKeyUpEventType(uint32) const
{
    return false;
}
bool DummyInputManager::IsKeyDownEventType(uint32) const
{
    return false;
}
void DummyInputManager::Update()
{
    while (run.load())
    {
        auto c = getchar();
        std::cout << "Pressed " << c << ", q : " << (int)'q' << std::endl;
        AddKey(c);
    }
}
void DummyInputManager::AddKey(int i)
{
    std::lock_guard<std::mutex> lk(kmutex);
    if (i == 'q')
        keys[KeyCodes::ESCAPE] = i;
}
bool DummyInputManager::IsKey(KeyCodes::Type key)
{
    std::lock_guard<std::mutex> lk(kmutex);
    return keys.count(key) != 0;
}
}  // namespace GameEngine
