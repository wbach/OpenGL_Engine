#include "InputManager.h"
#include <Logger/Log.h>
#include <algorithm>

namespace Input
{
InputManager::InputManager()
    : idCounter_(0)
{
    SetDefaultKeys();
}

InputManager::~InputManager()
{
    DEBUG_LOG("");
}

void InputManager::SetDefaultKeys()
{
    keyGameActions[GameActions::MOVE_FORWARD]          = KeyCodes::W;
    keyGameActions[GameActions::MOVE_BACKWARD]         = KeyCodes::S;
    keyGameActions[GameActions::MOVE_LEFT]             = KeyCodes::Q;
    keyGameActions[GameActions::MOVE_RIGHT]            = KeyCodes::R;
    keyGameActions[GameActions::TURN_LEFT]             = KeyCodes::A;
    keyGameActions[GameActions::TURN_RIGHT]            = KeyCodes::D;
    keyGameActions[GameActions::JUMP]                  = KeyCodes::SPACE;
    keyGameActions[GameActions::ATTACK_1]              = KeyCodes::LMOUSE;
    keyGameActions[GameActions::ATTACK_2]              = KeyCodes::RMOUSE;
    keyGameActions[GameActions::ATTACK_3]              = KeyCodes::Z;
    keyGameActions[GameActions::SPELL_1]               = KeyCodes::W;
    keyGameActions[GameActions::SPELL_2]               = KeyCodes::W;
    keyGameActions[GameActions::SPELL_3]               = KeyCodes::W;
    keyGameActions[GameActions::SPELL_4]               = KeyCodes::W;
    keyGameActions[GameActions::GUI_STATS_WINDOW]      = KeyCodes::C;
    keyGameActions[GameActions::GUI_INVENTORY_WINDOW]  = KeyCodes::I;
    keyGameActions[GameActions::GUI_PAUSE_MENU_WINDOW] = KeyCodes::ESCAPE;
    keyGameActions[GameActions::ITEM_1]                = KeyCodes::W;
    keyGameActions[GameActions::ITEM_2]                = KeyCodes::W;
    keyGameActions[GameActions::ITEM_3]                = KeyCodes::W;
    keyGameActions[GameActions::ITEM_4]                = KeyCodes::W;
    keyGameActions[GameActions::ITEM_5]                = KeyCodes::W;
    keyGameActions[GameActions::ITEM_6]                = KeyCodes::W;
    keyGameActions[GameActions::WORLD_MAP]             = KeyCodes::M;
}

uint32 InputManager::SubscribeOnKeyDown(KeyCodes::Type key, KeyPressedFunc func)
{
    auto id = idCounter_++;

    if (needToQueue_)
    {
        quque_.keyDownSubscribers_[key].insert({id, func});
    }
    else
    {
        subscribers_.keyDownSubscribers_[key].insert({id, func});
    }
    return id;
}

uint32 InputManager::SubscribeOnKeyUp(KeyCodes::Type key, KeyPressedFunc func)
{
    auto id = idCounter_++;

    if (needToQueue_)
    {
        quque_.keyUpSubscribers_[key].insert({id, func});
    }
    else
    {
        subscribers_.keyUpSubscribers_[key].insert({id, func});
    }

    return id;
}

uint32 InputManager::SubscribeOnAnyKeyPress(KeysPressedFunc func)
{
    auto id = idCounter_++;

    if (needToQueue_)
    {
        quque_.keysSubscribers_.insert({id, func});
    }
    else
    {
        subscribers_.keysSubscribers_.insert({id, func});
    }

    return id;
}

void InputManager::UnsubscribeAll()
{
    subscribers_.keysSubscribers_.clear();
    subscribers_.keyUpSubscribers_.clear();
    subscribers_.keyDownSubscribers_.clear();
}
void InputManager::UnsubscribeOnKeyDown(KeyCodes::Type key)
{
    if (quque_.keyDownSubscribers_.count(key) > 0)
    {
        quque_.keyDownSubscribers_.erase(key);
        return;
    }

    if (subscribers_.keyDownSubscribers_.count(key) == 0)
    {
        ERROR_LOG("Not existing subscribtion : {" + std::to_string(static_cast<int>(key)) + "}");
        return;
    }
    subscribers_.keyDownSubscribers_.erase(key);
}
void InputManager::UnsubscribeOnKeyUp(KeyCodes::Type key)
{
    if (quque_.keyDownSubscribers_.count(key) > 0)
    {
        quque_.keyUpSubscribers_.erase(key);
        return;
    }

    if (subscribers_.keyUpSubscribers_.count(key) == 0)
    {
        ERROR_LOG("Not existing subscribtion : {" + std::to_string(static_cast<int>(key)) + "}");
        return;
    }
    subscribers_.keyUpSubscribers_.erase(key);
}

void InputManager::UnsubscribeOnKeyDown(KeyCodes::Type key, uint32 id)
{
    if (quque_.keyDownSubscribers_.count(key) > 0)
    {
        auto& keys = quque_.keyDownSubscribers_.at(key);
        keys.erase(id);
        return;
    }

    if (subscribers_.keyDownSubscribers_.count(key) == 0)
    {
        ERROR_LOG("Not existing subscribtion : {" + std::to_string(static_cast<int>(key)) + ", " + std::to_string(id) +
                  "}");
        return;
    }
    auto& keys = subscribers_.keyDownSubscribers_.at(key);
    keys.erase(id);
}

void InputManager::UnsubscribeOnKeyUp(KeyCodes::Type key, uint32 id)
{
    if (quque_.keyUpSubscribers_.count(key) > 0)
    {
        auto& keys = quque_.keyUpSubscribers_.at(key);
        keys.erase(id);
        return;
    }

    if (subscribers_.keyUpSubscribers_.count(key) == 0)
    {
        ERROR_LOG("Not existing subscribtion : {" + std::to_string(static_cast<int>(key)) + ", " + std::to_string(id) +
                  "}");
        return;
    }

    auto& keys = subscribers_.keyUpSubscribers_.at(key);
    keys.erase(id);
}

void InputManager::UnsubscribeAnyKey(uint32 id)
{
    if (quque_.keysSubscribers_.count(id) > 0)
    {
        quque_.keysSubscribers_.erase(id);
        return;
    }

    if (subscribers_.keysSubscribers_.count(id) == 0)
    {
        ERROR_LOG("Not existing subscribtion : {" + std::to_string(id) + "}");
        return;
    }
    subscribers_.keysSubscribers_.erase(id);
}

void InputManager::StashSubscribers()
{
    stash_       = std::move(subscribers_);
    subscribers_ = Subscribers();
}
void InputManager::StashPopSubscribers()
{
    subscribers_ = stash_;
}
void InputManager::ExecuteOnKeyDown(KeyCodes::Type keyCode)
{
    needToQueue_ = true;

    if (subscribers_.keyDownSubscribers_.count(keyCode) > 0)
    {
        const auto& subscribers = subscribers_.keyDownSubscribers_.at(keyCode);

        for (const auto& subscriber : subscribers)
        {
            subscriber.second();
        }
    }
}
void InputManager::ExecuteOnKeyUp(KeyCodes::Type keyCode)
{
    needToQueue_ = true;

    if (subscribers_.keyUpSubscribers_.count(keyCode) > 0)
    {
        const auto& subscribers = subscribers_.keyUpSubscribers_.at(keyCode);

        for (const auto& subscriber : subscribers)
        {
            subscriber.second();
        }
    }
}
void InputManager::ExecuteAnyKey(KeyCodes::Type keyCode)
{
    needToQueue_ = true;

    for (const auto& keysSubscriber : subscribers_.keysSubscribers_)
    {
        keysSubscriber.second(keyCode);
    }
}
void InputManager::Unquque()
{
    for (const auto& keysSubscriber : quque_.keyDownSubscribers_)
    {
        for (const auto& keys : keysSubscriber.second)
        {
            if (subscribers_.keyDownSubscribers_.count(keysSubscriber.first) == 0)
            {
                subscribers_.keyDownSubscribers_.insert({keysSubscriber.first, {}});
            }
            subscribers_.keyDownSubscribers_.at(keysSubscriber.first).insert(keys);
        }
    }
    for (const auto& keysSubscriber : quque_.keyUpSubscribers_)
    {
        for (const auto& keys : keysSubscriber.second)
        {
            if (subscribers_.keyUpSubscribers_.count(keysSubscriber.first) == 0)
            {
                subscribers_.keyUpSubscribers_.insert({keysSubscriber.first, {}});
            }
            subscribers_.keyUpSubscribers_.at(keysSubscriber.first).insert(keys);
        }
    }

    for (const auto& keysSubscriber : quque_.keysSubscribers_)
    {
        subscribers_.keysSubscribers_.insert(keysSubscriber);
    }

    quque_       = Subscribers();
    needToQueue_ = false;
}
}  // namespace Input
