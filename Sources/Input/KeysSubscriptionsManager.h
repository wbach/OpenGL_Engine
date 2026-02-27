#pragma once
#include "InputManager.h"

namespace Input
{
class ENGINE_API KeysSubscriptionsManager
{
public:
    KeysSubscriptionsManager(InputManager& inputManager);
    ~KeysSubscriptionsManager();

    void operator=(uint32 id);
    void AddSubscribtion(uint32);
    void UnsubscribeKeys();
    void Clear();

private:
    InputManager& inputManager_;
    std::vector<uint32> subscriptions_;
};
}  // namespace Input
