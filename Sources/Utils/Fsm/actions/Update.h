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
};
}  // namespace StateMachine
}  // namespace Utils
