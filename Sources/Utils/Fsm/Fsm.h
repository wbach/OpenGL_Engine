#pragma once
/*
https://sii.pl/blog/implementing-a-state-machine-in-c17/
https://github.com/AdamsPL/state-machine
*/
#include <Logger/Log.h>

#include <tuple>
#include <variant>

namespace Utils
{
namespace StateMachine
{
template <typename... States>
class Fsm
{
public:
    Fsm() = default;

    Fsm(States... states)
        : states(std::move(states)...)
    {
    }

    template <typename State>
    State& transitionTo()
    {
        State& state = std::get<State>(states);
        currentState = &state;
        return state;
    }

    template <typename Event>
    void handle(const Event& event)
    {
        handleBy(event, *this);
    }

    template <typename Event, typename Machine>
    void handleBy(const Event& event, Machine& machine)
    {
#ifdef NOREALTIME_LOG_ENABLED
        DEBUG_LOG("Handle event : " + typeName<Event>());
#endif
        auto passEventToState = [&machine, &event](auto statePtr)
        {
            auto action = statePtr->handle(event);
            action.execute(machine, *statePtr, event);
        };
        std::visit(passEventToState, currentState);
    }

    std::tuple<States...> states;
    std::variant<States*...> currentState{&std::get<0>(states)};
};
}  // namespace StateMachine
}  // namespace Utils
