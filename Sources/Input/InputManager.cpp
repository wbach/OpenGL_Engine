#include "InputManager.h"
#include <Logger/Log.h>
#include <algorithm>

namespace Input
{
InputManager::InputManager()
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

std::size_t InputManager::SubscribeOnKeyDown(KeyCodes::Type key, KeyPressedFunc func)
{
    subscribers_.keyDownSubscribers_[key].push_back(func);
    return subscribers_.keyDownSubscribers_.at(key).size() - 1;
}

std::size_t InputManager::SubscribeOnKeyUp(KeyCodes::Type key, KeyPressedFunc func)
{
    subscribers_.keyUpSubscribers_[key].push_back(func);
    return subscribers_.keyUpSubscribers_.at(key).size() - 1;
}

std::size_t InputManager::SubscribeOnAnyKeyPress(KeysPressedFunc func)
{
    subscribers_.keysSubscribers_.push_back(func);
    return subscribers_.keysSubscribers_.size() - 1;
}

void InputManager::UnsubscribeAll()
{
    subscribers_.keysSubscribers_.clear();
    subscribers_.keyUpSubscribers_.clear();
    subscribers_.keyDownSubscribers_.clear();
}
void InputManager::UnsubscribeOnKeyDown(KeyCodes::Type key)
{
    if (subscribers_.keyDownSubscribers_.count(key) == 0)
        return;
    subscribers_.keyDownSubscribers_.erase(key);
}
void InputManager::UnsubscribeOnKeyUp(KeyCodes::Type key)
{
    if (subscribers_.keyUpSubscribers_.count(key) == 0)
        return;
    subscribers_.keyUpSubscribers_.erase(key);
}

void InputManager::UnsubscribeOnKeyDown(KeyCodes::Type key, std::size_t i)
{
    if (subscribers_.keyDownSubscribers_.count(key) == 0)
        return;

    auto& list = subscribers_.keyDownSubscribers_.at(key);
    list.erase(list.begin() + i);
}

void InputManager::UnsubscribeOnKeyUp(KeyCodes::Type key, std::size_t i)
{
    if (subscribers_.keyUpSubscribers_.count(key) == 0)
        return;

    auto& list = subscribers_.keyUpSubscribers_.at(key);
    list.erase(list.begin() + i);
}
void InputManager::StashSubscribers()
{
    stash_ = subscribers_;
    subscribers_ = Subscribers();
}
void InputManager::StashPopSubscribers()
{
    subscribers_ = stash_;
}
}  // namespace Input
