#pragma once
#include <Logger/Log.h>

namespace Utils
{
namespace StateMachine
{
class Update
{
public:
    template <typename Machine, typename State, typename Event>
    void execute(Machine& machine, State& state, const Event& event)
    {
        update(state, event);
        post(state);
    }

private:
    void update(...)
    {
    }

    template <typename State, typename Event>
    auto update(State& state, const Event& event) -> decltype(state.update(event))
    {
        return state.update(event);
    }

    template <typename... Args>
    void post(Args&...)
    {
    }

    template <typename State>
    auto post(State& state) -> decltype(state.postUpdate())
    {
        return state.postUpdate();
    }

};
}  // namespace StateMachine
}  // namespace Utils
