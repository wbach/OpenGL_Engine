#pragma once
#include "InputManager.h"

namespace Input
{
class KeysSubscriptionsManager
{
public:
    KeysSubscriptionsManager(InputManager& inputManager);
    ~KeysSubscriptionsManager();

    void operator=(uint32 id);
    void AddSubscribtion(uint32);
    void UnsubscribeKeys();

private:
    InputManager& inputManager_;
    std::vector<uint32> subscriptions_;
};
}  // namespace Input
