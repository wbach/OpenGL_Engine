#pragma once
#include <atomic>
#include "Input/InputManager.h"
#include "Mutex.hpp"
#include "Thread.hpp"

namespace GameEngine
{
class DummyInputManager : public Input::InputManager
{
public:
    DummyInputManager();
    ~DummyInputManager();
    virtual bool GetKey(KeyCodes::Type) override;
    virtual bool GetMouseKey(KeyCodes::Type) override;
    virtual vec2i CalcualteMouseMove() override;
    virtual vec2i GetPixelMousePosition() override;
    virtual vec2 GetMousePosition() override;

    virtual void SetCursorPosition(int, int) override;
    virtual void SetKeyToBuffer(int, bool) override;
    virtual void ClearKeyBuffer() override;
    virtual void GetPressedKeys() override;
    virtual void ProcessKeysEvents() override;
    virtual void ShowCursor(bool);

private:
    void Update();
    void AddKey(int i);
    bool IsKey(KeyCodes::Type key);

private:
    std::unordered_map<KeyCodes::Type, int> keys;
    std::thread upThread;
    std::mutex kmutex;
    std::atomic_bool run;
};

typedef std::shared_ptr<DummyInputManager> DummyInputManagerPtr;
}  // GameEngine
