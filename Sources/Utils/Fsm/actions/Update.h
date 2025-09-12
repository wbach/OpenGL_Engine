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
    template <typename... Args>
    void update(Args&...)
    {
    }

    template <typename State, typename Event>
    auto update(State& state, const Event& event) -> decltype(state.update(event))
    {
#ifdef NOREALTIME_LOG_ENABLED
        /* LOG TO FIX*/  LOG_ERROR << ("Update state: " + typeName<State>() + ", event: " + typeName<Event>());
#endif
        return state.update(event);
    }

    template <typename... Args>
    void post(Args&...)
    {
    }

    template <typename State>
    auto post(State& state) -> decltype(state.postUpdate())
    {
#ifdef NOREALTIME_LOG_ENABLED
        /* LOG TO FIX*/  LOG_ERROR << ("Post update state: " + typeName<State>());
#endif
        return state.postUpdate();
    }
};
}  // namespace StateMachine
}  // namespace Utils
