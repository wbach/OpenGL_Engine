#pragma once
#include <functional>
#include <vector>

#include "Event.h"

namespace GameEngine
{
namespace Components
{
struct EndAnimationTransitionEvent : public Event
{
    EndAnimationTransitionEvent() = default;
    EndAnimationTransitionEvent(const std::vector<std::function<void()>>& onEntryCallbacks)
        : onEntryCallbacks{onEntryCallbacks}
    {
    }
    void process() override;
    const std::vector<std::function<void()>> onEntryCallbacks;
};
}  // namespace Components
}  // namespace GameEngine
