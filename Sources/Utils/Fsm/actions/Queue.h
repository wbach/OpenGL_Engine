#pragma once

namespace Utils
{
namespace StateMachine
{
struct Queue
{
    template <typename Machine, typename State, typename Event>
    void execute(Machine&, State& state, const Event& event)
    {
        state.pushEventToQueue(event);
    }
};
template<typename EventToRemove>
struct RemoveFromQueue
{
    template <typename Machine, typename State, typename Event>
    void execute(Machine&, State& state, const Event& event)
    {
        state.removeEventFromQueue(EventToRemove{});
    }
};
}  // namespace StateMachine
}  // namespace Utils

