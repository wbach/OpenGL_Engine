#pragma once
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <unordered_map>
#include "GameActions.h"
#include "KeyCodes.h"
#include "Types.h"

namespace Input
{
typedef std::function<void()> KeyPressedFunc;
typedef std::list<KeyPressedFunc> KeySubscribers;
typedef std::unordered_map<KeyCodes::Type, KeySubscribers> KeyPressedSubscribers;
typedef std::function<void(KeyCodes::Type key)> KeysPressedFunc;
typedef std::list<KeysPressedFunc> KeysSubscribers;

class InputManager
{
public:
    InputManager();
    virtual ~InputManager();
    void SetDefaultKeys();
    virtual bool GetKey(KeyCodes::Type i)        = 0;
    virtual bool GetMouseKey(KeyCodes::Type key) = 0;
    virtual vec2i CalcualteMouseMove()           = 0;
    virtual vec2i GetMousePosition()             = 0;

    virtual void SetCursorPosition(int x, int y)     = 0;
    virtual void SetKeyToBuffer(int key, bool value) = 0;
    virtual void ClearKeyBuffer()                    = 0;
    virtual void GetPressedKeys()                    = 0;
    virtual void ProcessKeysEvents()                 = 0;

    virtual void ShowCursor(bool){};

    void SubscribeOnKeyDown(KeyCodes::Type key, KeyPressedFunc func);
    void SubscribeOnKeyUp(KeyCodes::Type key, KeyPressedFunc func);
    void SubscribeOnAnyKeyPress(KeysPressedFunc func);

    void UnsubscribeOnKeyDown(KeyCodes::Type key);
    void UnsubscribeOnKeyUp(KeyCodes::Type key);

    void UnsubscribeAll();

public:
    std::unordered_map<GameActions::Type, KeyCodes::Type> keyGameActions;

protected:
    std::set<KeyCodes::Type> keyBuffer;
    KeyPressedSubscribers keyDownSubscribers_;
    KeyPressedSubscribers keyUpSubscribers_;
    KeysSubscribers keysSubscribers_;
};

typedef std::shared_ptr<InputManager> InputManagerPtr;
}  // namespace Input
