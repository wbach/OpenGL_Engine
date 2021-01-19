#pragma once
#include <optional>
#include <vector>
#include <memory>
#include "Common.h"
#include "IState.h"

namespace GameEngine
{
namespace Components
{
struct Event;

struct StateMachine
{
    ~StateMachine();
    std::optional<Pose> update(float);
    void handle(std::unique_ptr<Event>);
    void processEvents();
    const std::string& getCurrentAnimationClipName() const;

    std::vector<std::unique_ptr<Event>> events_;
    std::unique_ptr<IState> currentState_;
};
}  // namespace Components
}  // namespace GameEngine
