#include "InputManager.h"

#include <Logger/Log.h>

#include <algorithm>
#include <mutex>

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
    : idCounter_(0)
    , needToQueue_{false}
    , stashedSubsribtions_(false)
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
}

bool InputManager::GetKey(GameAction action)
{
    if (keyGameActions_.count(action))
    {
        return GetKey(keyGameActions_.at(action));
    }
    // else
    //{
    //     /* LOG TO FIX*/  LOG_ERROR << ("Action not registered. " + std::to_string(static_cast<int>(action)));
    // }
    return false;
}

void InputManager::AddEvent(KeyPressedFunc eventFunc)
{
    std::lock_guard<std::mutex> lk(eventMutex);
    events_.push_back(eventFunc);
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

    if (UnsubscribeImpl(quque_.keyDownSubscribers_, id)) return;
    if (UnsubscribeImpl(quque_.keyUpSubscribers_, id)) return;
    if (UnsubscribeImpl(quque_.keysSubscribers_, id)) return;
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
    auto ququeKeyIter = quque_.keyDownSubscribers_.find(key);
    if (ququeKeyIter != quque_.keyDownSubscribers_.end())
    {
        quque_.keyDownSubscribers_.erase(ququeKeyIter);
        return;
    }

    auto subscribersKeyIter = subscribers_.keyDownSubscribers_.find(key);
    if (subscribersKeyIter != subscribers_.keyDownSubscribers_.end())
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Not existing subscribtion : {" + std::to_string(static_cast<int>(key)) + "}");
        return;
    }
    subscribers_.keyDownSubscribers_.erase(subscribersKeyIter);
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
        /* LOG TO FIX*/ LOG_ERROR << ("Not existing subscribtion : {" + std::to_string(static_cast<int>(key)) + "}");
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
        /* LOG TO FIX*/ LOG_ERROR << ("Not existing subscribtion : {" + std::to_string(static_cast<int>(key)) + ", " +
                                      std::to_string(id) + "}");
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
        /* LOG TO FIX*/ LOG_ERROR << ("Not existing subscribtion : {" + std::to_string(static_cast<int>(key)) + ", " +
                                      std::to_string(id) + "}");
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
        /* LOG TO FIX*/ LOG_ERROR << ("Not existing subscribtion : {" + std::to_string(id) + "}");
        return;
    }
    subscribers_.keysSubscribers_.erase(id);
}

uint32 InputManager::SubscribeOnKeyDown(GameAction action, KeyPressedFunc func)
{
    if (keyGameActions_.count(action))
    {
        return SubscribeOnKeyDown(keyGameActions_.at(action), func);
    }
    else
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Action not registered. " + std::to_string(static_cast<int>(action)));
    }
    return 0;
}

uint32 InputManager::SubscribeOnKeyUp(GameAction action, KeyPressedFunc func)
{
    if (keyGameActions_.count(action))
    {
        return SubscribeOnKeyUp(keyGameActions_.at(action), func);
    }
    else
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Action not registered. " + std::to_string(static_cast<int>(action)));
    }
    return 0;
}

void InputManager::UnsubscribeOnKeyDown(GameAction action, uint32 id)
{
    if (keyGameActions_.count(action))
    {
        UnsubscribeOnKeyDown(keyGameActions_.at(action), id);
    }
    else
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Action not registered. " + std::to_string(static_cast<int>(action)));
    }
}

void InputManager::UnsubscribeOnKeyUp(GameAction action, uint32 id)
{
    if (keyGameActions_.count(action))
    {
        UnsubscribeOnKeyUp(keyGameActions_.at(action), id);
    }
    else
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Action not registered. " + std::to_string(static_cast<int>(action)));
    }
}

void InputManager::UnsubscribeOnKeyDown(GameAction action)
{
    if (keyGameActions_.count(action))
    {
        UnsubscribeOnKeyDown(keyGameActions_.at(action));
    }
    else
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Action not registered. " + std::to_string(static_cast<int>(action)));
    }
}

void InputManager::UnsubscribeOnKeyUp(GameAction action)
{
    if (keyGameActions_.count(action))
    {
        UnsubscribeOnKeyUp(keyGameActions_.at(action));
    }
    else
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Action not registered. " + std::to_string(static_cast<int>(action)));
    }
}

void InputManager::StashSubscribers()
{
    if (stashedSubsribtions_)
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Multiple stash subscribtions, losing last one");
    }

    stashedSubsribtions_ = true;
    stash_               = std::move(subscribers_);
    subscribers_         = Subscribers();
}
void InputManager::StashPopSubscribers()
{
    subscribers_         = stash_;
    stashedSubsribtions_ = false;
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

    auto iter = std::find_if(keyEvents_.begin(), keyEvents_.end(), [eventType, key](const KeyEvent& keyEvent)
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
        Unquque();
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
}  // namespace Input
