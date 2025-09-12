#pragma once
#include <vector>

#include "CharacterControllerCommonDefs.h"

namespace GameEngine::Components
{
class BaseState
{
public:
    BaseState(FsmContext&);
    virtual ~BaseState() = default;

    void update(float);
    void flushEvents();

    template <typename Event>
    void pushEventToQueue(const Event& event)
    {
        queue_.push_back(event);
    }
    template <typename Event>
    void removeEventFromQueue(const Event& event)
    {
        queue_.erase(
            std::remove_if(queue_.begin(), queue_.end(), [](const auto& event) { return std::holds_alternative<Event>(event); }));
    }

    template <typename... Events>
    bool isAnyOfEventQueued()
    {
        auto iter = std::find_if(queue_.begin(), queue_.end(),
                                 [](const auto& event) { return (std::holds_alternative<Events>(event) or ...); });

        return iter != queue_.end();
    }

protected:
    FsmContext& context_;
    std::vector<CharacterControllerEvent>& queue_;
};
}  // namespace GameEngine::Components
