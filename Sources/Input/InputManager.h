#pragma once
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>

#include "GameActions.h"
#include "KeyCodes.h"
#include "Types.h"

namespace Input
{
typedef std::function<void()> KeyPressedFunc;
typedef std::unordered_map<uint32, KeyPressedFunc> KeySubscribers;
typedef std::unordered_map<KeyCodes::Type, KeySubscribers> KeyPressedSubscribers;
typedef std::function<void(KeyCodes::Type key)> KeysPressedFunc;
typedef std::unordered_map<uint32, KeysPressedFunc> KeysSubscribers;
typedef std::pair<uint32, uint32> KeyEvent;

class InputManager
{
    struct Subscribers
    {
        KeyPressedSubscribers keyDownSubscribers_;
        KeyPressedSubscribers keyUpSubscribers_;
        KeysSubscribers keysSubscribers_;
    };

public:
    InputManager();
    virtual ~InputManager();
    void SetDefaultKeys();
    virtual bool GetKey(KeyCodes::Type i)        = 0;
    virtual bool GetMouseKey(KeyCodes::Type key) = 0;
    virtual void SetReleativeMouseMode(bool)     = 0;
    virtual vec2i CalcualteMouseMove()           = 0;
    virtual vec2i GetPixelMousePosition()        = 0;
    virtual vec2 GetMousePosition()              = 0;

    virtual void SetCursorPosition(int x, int y)     = 0;
    virtual void SetKeyToBuffer(int key, bool value) = 0;
    virtual void ClearKeyBuffer()                    = 0;
    virtual void GetPressedKeys()                    = 0;
    virtual void ShowCursor(bool)                    = 0;

    void AddKeyEvent(uint32 eventType, uint32 key);
    void ProcessKeysEvents();
    void StashSubscribers();
    void StashPopSubscribers();

    uint32 SubscribeOnKeyDown(KeyCodes::Type key, KeyPressedFunc func);
    uint32 SubscribeOnKeyUp(KeyCodes::Type key, KeyPressedFunc func);
    uint32 SubscribeOnAnyKeyPress(KeysPressedFunc func);

    void UnsubscribeOnKeyDown(KeyCodes::Type key);
    void UnsubscribeOnKeyUp(KeyCodes::Type key);

    void UnsubscribeOnKeyDown(KeyCodes::Type key, uint32);
    void UnsubscribeOnKeyUp(KeyCodes::Type key, uint32);
    void UnsubscribeAnyKey(uint32);

    void Unsubscribe(uint32);
    void UnsubscribeAll();

protected:
    virtual KeyCodes::Type ConvertCode(uint32) const = 0;
    virtual bool IsKeyUpEventType(uint32) const      = 0;
    virtual bool IsKeyDownEventType(uint32) const    = 0;
    bool FindEvent(uint32 eventType, uint32 key);
    std::optional<KeyEvent> GetEvent();
    void UpdateMouseState(uint32 code, bool state);
    bool GetMouseState(uint32 code);

private:
    void ExecuteOnKeyDown(KeyCodes::Type);
    void ExecuteOnKeyUp(KeyCodes::Type);
    void ExecuteAnyKey(KeyCodes::Type);
    void Unquque();

public:
    std::unordered_map<GameActions::Type, KeyCodes::Type> keyGameActions;

protected:
    std::set<KeyCodes::Type> keyBuffer;
    std::list<KeyEvent> keyEvents_;
    Subscribers subscribers_;
    Subscribers stash_;
    Subscribers quque_;
    uint32 idCounter_;
    bool needToQueue_;
};
}  // namespace Input
