#pragma once

namespace GameEngine
{
namespace Components
{
struct StateMachine;
struct Event
{
    virtual ~Event()       = default;
    virtual void process() = 0;
    StateMachine* machine{nullptr};
};
}  // namespace Components
}  // namespace GameEngine
