#include "InputManager.h"

#include <Logger/Log.h>

#include <algorithm>
#include <mutex>

#include "Input/KeyCodes.h"
#include "magic_enum/magic_enum.hpp"

namespace Input
{
namespace
{
std::mutex keyEventMutex;
std::mutex eventMutex;

struct MouseState
{
    bool lButton_ = false;
    bool rButton_ = false;
    bool mButton_ = false;
};
MouseState mouseState;
}  // namespace

InputManager::InputManager()
{
    SetDefaultKeys();
}

InputManager::~InputManager()
{
    LOG_DEBUG << "destructor";
}

void InputManager::SetDefaultKeys()
{
    RegisterGameAction(GameAction::MOVE_FORWARD, KeyCodes::W);
    RegisterGameAction(GameAction::MOVE_BACKWARD, KeyCodes::S);
    RegisterGameAction(GameAction::MOVE_LEFT, KeyCodes::A);
    RegisterGameAction(GameAction::MOVE_RIGHT, KeyCodes::D);
    RegisterGameAction(GameAction::TURN_LEFT, KeyCodes::Q);
    RegisterGameAction(GameAction::TURN_RIGHT, KeyCodes::E);
    RegisterGameAction(GameAction::JUMP, KeyCodes::SPACE);
    RegisterGameAction(GameAction::DODGE_DIVE, KeyCodes::LCTRL);
    RegisterGameAction(GameAction::DODGE, KeyCodes::LALT);
    RegisterGameAction(GameAction::WALK, KeyCodes::Z);
    RegisterGameAction(GameAction::SPRINT, KeyCodes::LSHIFT);
    RegisterGameAction(GameAction::EQUIP_DISARM, KeyCodes::F);
    RegisterGameAction(GameAction::CROUCH, KeyCodes::C);
    RegisterGameAction(GameAction::AIM, KeyCodes::RMOUSE);
    RegisterGameAction(GameAction::ATTACK, KeyCodes::LMOUSE);
    RegisterGameAction(GameAction::CAM_FORWARD, KeyCodes::UARROW);
    RegisterGameAction(GameAction::CAM_BACKWARD, KeyCodes::DARROW);
    RegisterGameAction(GameAction::CAM_LEFT, KeyCodes::LARROW);
    RegisterGameAction(GameAction::CAM_RIGHT, KeyCodes::RARROW);
    RegisterGameAction(GameAction::CAM_PITCH_UP, KeyCodes::NUM1);
    RegisterGameAction(GameAction::CAM_PITCH_DOWN, KeyCodes::NUM2);
    RegisterGameAction(GameAction::DIALOG_START, KeyCodes::G);
    RegisterGameAction(GameAction::QUEST_VIEW, KeyCodes::TAB);
    RegisterGameAction(GameAction::INVENTORY_VIEW, KeyCodes::I);
}

bool InputManager::GetKey(GameAction action)
{
    auto iter = keyGameActions_.find(action);
    if (iter != keyGameActions_.end())
    {
        return GetKey(iter->second);
    }
    return false;
}

void InputManager::AddEvent(KeyPressedFunc eventFunc)
{
    std::lock_guard<std::mutex> lk(eventMutex);
    events_.push_back(eventFunc);
}

uint32 InputManager::SubscribeOnKeyDown(KeyCodes::Type key, KeyPressedFunc func)
{
    auto id = idPool_.getId();
    subscribers_.keyDownSubscribers_[key].insert({id, func});
    return id;
}

uint32 InputManager::SubscribeOnKeyUp(KeyCodes::Type key, KeyPressedFunc func)
{
    auto id = idPool_.getId();
    subscribers_.keyUpSubscribers_[key].insert({id, func});
    return id;
}

uint32 InputManager::SubscribeOnAnyKeyPress(KeysPressedFunc func)
{
    auto id = idPool_.getId();
    subscribers_.keysSubscribers_.insert({id, func});
    return id;
}

bool UnsubscribeImpl(KeyPressedSubscribers& sub, uint32 id)
{
    for (auto& k : sub)
    {
        if (k.second.count(id) > 0)
        {
            k.second.erase(id);
            return true;
        }
    }

    return false;
}

bool UnsubscribeImpl(KeysSubscribers& sub, uint32 id)
{
    if (sub.count(id) > 0)
    {
        sub.erase(id);
        return true;
    }

    return false;
}

void InputManager::Unsubscribe(uint32 id)
{
    // clang-format off
    if (UnsubscribeImpl(subscribers_.keyDownSubscribers_, id)) return;
    if (UnsubscribeImpl(subscribers_.keyUpSubscribers_, id)) return;
    if (UnsubscribeImpl(subscribers_.keysSubscribers_, id)) return;
    // clang-format on
}

void InputManager::UnsubscribeAll()
{
    subscribers_.keysSubscribers_.clear();
    subscribers_.keyUpSubscribers_.clear();
    subscribers_.keyDownSubscribers_.clear();
}

const std::unordered_map<GameAction, KeyCodes::Type>& InputManager::GetKeysGameActions() const
{
    return keyGameActions_;
}

void InputManager::ProcessEvents()
{
    if (events_.empty())
        return;

    std::lock_guard<std::mutex> lk(eventMutex);
    for (auto& event : events_)
    {
        event();
    }
    events_.clear();
}
void InputManager::UnsubscribeOnKeyDown(KeyCodes::Type key)
{
    auto iter = subscribers_.keyDownSubscribers_.find(key);
    if (iter == subscribers_.keyDownSubscribers_.end())
    {
        LOG_WARN << "Not existing subscribtion : {" << magic_enum::enum_name(key);
        return;
    }
    subscribers_.keyDownSubscribers_.erase(iter);
}
void InputManager::UnsubscribeOnKeyUp(KeyCodes::Type key)
{
    auto iter = subscribers_.keyUpSubscribers_.find(key);
    if (iter == subscribers_.keyUpSubscribers_.end())
    {
        LOG_WARN << "Not existing subscribtion : {" << magic_enum::enum_name(key);
        return;
    }
    subscribers_.keyUpSubscribers_.erase(iter);
}

void InputManager::UnsubscribeOnKeyDown(KeyCodes::Type key, uint32 id)
{
    auto iter = subscribers_.keyDownSubscribers_.find(key);
    if (iter == subscribers_.keyDownSubscribers_.end())
    {
        LOG_WARN << "Not existing subscribtion : {" << magic_enum::enum_name(key) << ", " << id << "}";
        return;
    }
    auto& keys = iter->second;
    keys.erase(id);
}

void InputManager::UnsubscribeOnKeyUp(KeyCodes::Type key, uint32 id)
{
    auto iter = subscribers_.keyUpSubscribers_.find(key);
    if (subscribers_.keyUpSubscribers_.end() == iter)
    {
        LOG_WARN << "Not existing subscribtion : {" << magic_enum::enum_name(key) << ", " << id << "}";
        return;
    }

    auto& keys = iter->second;
    keys.erase(id);
}

void InputManager::UnsubscribeAnyKey(uint32 id)
{
    auto iter = subscribers_.keysSubscribers_.find(id);
    if (subscribers_.keysSubscribers_.end() == iter)
    {
        LOG_WARN << "Not existing subscribtion : {" << id << "}";
        return;
    }
    subscribers_.keysSubscribers_.erase(iter);
}

uint32 InputManager::SubscribeOnKeyDown(GameAction action, KeyPressedFunc func)
{
    auto iter = keyGameActions_.find(action);
    if (keyGameActions_.end() != iter)
    {
        return SubscribeOnKeyDown(iter->second, func);
    }
    else
    {
        LOG_WARN << "Action not registered. " << magic_enum::enum_name(action);
    }
    return 0;
}

uint32 InputManager::SubscribeOnKeyUp(GameAction action, KeyPressedFunc func)
{
    auto iter = keyGameActions_.find(action);
    if (keyGameActions_.end() != iter)
    {
        return SubscribeOnKeyUp(iter->second, func);
    }
    else
    {
        LOG_WARN << "Action not registered. " << magic_enum::enum_name(action);
    }
    return 0;
}

void InputManager::UnsubscribeOnKeyDown(GameAction action, uint32 id)
{
    auto iter = keyGameActions_.find(action);
    if (keyGameActions_.end() != iter)
    {
        UnsubscribeOnKeyDown(iter->second, id);
    }
    else
    {
        LOG_WARN << "Action not registered. " << magic_enum::enum_name(action);
    }
}

void InputManager::UnsubscribeOnKeyUp(GameAction action, uint32 id)
{
    auto iter = keyGameActions_.find(action);
    if (keyGameActions_.end() != iter)
    {
        UnsubscribeOnKeyUp(iter->second, id);
    }
    else
    {
        LOG_WARN << "Action not registered. " << magic_enum::enum_name(action);
    }
}

void InputManager::UnsubscribeOnKeyDown(GameAction action)
{
    auto iter = keyGameActions_.find(action);
    if (keyGameActions_.end() != iter)
    {
        UnsubscribeOnKeyDown(iter->second);
    }
    else
    {
        LOG_WARN << "Action not registered. " << magic_enum::enum_name(action);
    }
}

void InputManager::UnsubscribeOnKeyUp(GameAction action)
{
    auto iter = keyGameActions_.find(action);
    if (keyGameActions_.end() != iter)
    {
        UnsubscribeOnKeyUp(iter->second);
    }
    else
    {
        LOG_WARN << "Action not registered. " << magic_enum::enum_name(action);
    }
}

void InputManager::StashSubscribers()
{
    stash_.push_back(std::move(subscribers_));
    subscribers_ = Subscribers();
}
void InputManager::StashPopSubscribers()
{
    if (stash_.empty())
        return;

    subscribers_ = stash_.back();
    stash_.pop_back();
}
void InputManager::Exectute(KeyPressedSubscribers& subscribers, KeyCodes::Type keyCode)
{
    auto iter = subscribers.find(keyCode);
    if (iter != subscribers.end())
    {
        auto& originalMap      = iter->second;
        auto copyOfSubscribers = originalMap;

        for (const auto& [id, callback] : copyOfSubscribers)
        {
            if (originalMap.find(id) != originalMap.end())
            {
                callback();
            };
        }
    }
}
void InputManager::ExecuteOnKeyDown(KeyCodes::Type keyCode)
{
    Exectute(subscribers_.keyDownSubscribers_, keyCode);
}
void InputManager::ExecuteOnKeyUp(KeyCodes::Type keyCode)
{
    Exectute(subscribers_.keyUpSubscribers_, keyCode);
}
void InputManager::ExecuteAnyKey(KeyCodes::Type keyCode)
{
    auto copy = subscribers_.keysSubscribers_;
    for (const auto& [id, callback] : copy)
    {
        if (subscribers_.keysSubscribers_.find(id) != subscribers_.keysSubscribers_.end())
        {
            callback(keyCode);
        }
    }
}

void InputManager::AddKeyEvent(uint32 eventType, uint32 key)
{
    if (FindEvent(eventType, key))
        return;

    std::lock_guard<std::mutex> lk(keyEventMutex);
    keyEvents_.push_back({eventType, key});
}

bool InputManager::FindEvent(uint32 eventType, uint32 key)
{
    std::lock_guard<std::mutex> lk(keyEventMutex);

    auto iter = std::find_if(keyEvents_.begin(), keyEvents_.end(),
                             [eventType, key](const KeyEvent& keyEvent)
                             { return keyEvent.first == eventType and key == keyEvent.second; });

    return iter != keyEvents_.end();
}

std::optional<KeyEvent> InputManager::GetEvent()
{
    std::lock_guard<std::mutex> lk(keyEventMutex);

    if (keyEvents_.empty())
        return std::nullopt;

    auto e = keyEvents_.front();
    keyEvents_.pop_front();
    return std::move(e);
}

void InputManager::ProcessKeysEvents()
{
    while (true)
    {
        auto incomingEvent = GetEvent();

        if (not incomingEvent)
            return;

        auto type  = incomingEvent->first;
        auto value = incomingEvent->second;

        auto keyCode = ConvertCode(value);

        if (IsKeyDownEventType(type))
        {
            UpdateMouseState(keyCode, true);
            ExecuteOnKeyDown(keyCode);
            ExecuteAnyKey(keyCode);
        }
        else if (IsKeyUpEventType(type))
        {
            UpdateMouseState(keyCode, false);
            ExecuteOnKeyUp(keyCode);
        }

        ProcessEvents();
    }
}

void InputManager::UpdateMouseState(uint32 keyCode, bool state)
{
    if (keyCode == KeyCodes::LMOUSE)
    {
        mouseState.lButton_ = state;
    }
    else if (keyCode == KeyCodes::RMOUSE)
    {
        mouseState.rButton_ = state;
    }
    else if (keyCode == KeyCodes::MOUSE_WHEEL)
    {
        mouseState.mButton_ = state;
    }
}
bool InputManager::GetMouseState(uint32 keyCode)
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

void InputManager::RegisterGameAction(GameAction action, KeyCodes::Type key)
{
    keyGameActions_.insert({action, key});
}
bool InputManager::IsCursorVisible() const
{
    return isCursorVisible;
}
}  // namespace Input
