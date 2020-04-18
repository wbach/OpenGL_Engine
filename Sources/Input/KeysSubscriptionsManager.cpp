#include "KeysSubscriptionsManager.h"

namespace Input
{
KeysSubscriptionsManager::KeysSubscriptionsManager(InputManager& inputManager)
    : inputManager_(inputManager)
{
}
KeysSubscriptionsManager::~KeysSubscriptionsManager()
{
    UnsubscribeKeys();
}
void KeysSubscriptionsManager::operator=(uint32 id)
{
    subscriptions_.push_back(id);
}
void KeysSubscriptionsManager::AddSubscribtion(uint32 id)
{
    subscriptions_.push_back(id);
}
void KeysSubscriptionsManager::UnsubscribeKeys()
{
    for (auto& ks : subscriptions_)
    {
        inputManager_.Unsubscribe(ks);
    }
    subscriptions_.clear();
}
void KeysSubscriptionsManager::Clear()
{
    subscriptions_.clear(); // after stash pop subscribtion can be invalid
}
}  // namespace Input
