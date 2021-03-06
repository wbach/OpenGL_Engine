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

    bool GetKey(GameAction);

    void AddEvent(KeyPressedFunc);
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

    uint32 SubscribeOnKeyDown(GameAction, KeyPressedFunc func);
    uint32 SubscribeOnKeyUp(GameAction, KeyPressedFunc func);
    void UnsubscribeOnKeyDown(GameAction, uint32);
    void UnsubscribeOnKeyUp(GameAction, uint32);
    void UnsubscribeOnKeyDown(GameAction);
    void UnsubscribeOnKeyUp(GameAction);

    void Unsubscribe(uint32);
    void UnsubscribeAll();
    const std::unordered_map<GameAction, KeyCodes::Type>& GetKeysGameActions() const;

protected:
    virtual KeyCodes::Type ConvertCode(uint32) const = 0;
    virtual bool IsKeyUpEventType(uint32) const      = 0;
    virtual bool IsKeyDownEventType(uint32) const    = 0;
    void ProcessEvents();
    bool FindEvent(uint32 eventType, uint32 key);
    std::optional<KeyEvent> GetEvent();
    void UpdateMouseState(uint32 code, bool state);
    bool GetMouseState(uint32 code);

private:
    void RegisterGameAction(GameAction, KeyCodes::Type);
    void ExecuteOnKeyDown(KeyCodes::Type);
    void ExecuteOnKeyUp(KeyCodes::Type);
    void ExecuteAnyKey(KeyCodes::Type);
    void Unquque();

protected:
    std::unordered_map<GameAction, KeyCodes::Type> keyGameActions_;
    std::set<KeyCodes::Type> keyBuffer;
    std::vector<KeyPressedFunc> events_;
    std::list<KeyEvent> keyEvents_;
    Subscribers subscribers_;
    Subscribers stash_;
    Subscribers quque_;
    uint32 idCounter_;
    bool needToQueue_;
    bool stashedSubsribtions_;
};
}  // namespace Input
