#pragma once
/*
https://sii.pl/blog/implementing-a-state-machine-in-c17/
https://github.com/AdamsPL/state-machine
*/
#include <Logger/Log.h>

#include <list>
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
        previousState.push_back(currentState);
        if (previousState.size() > 10)
        {
            previousState.pop_front();
        }

        State& state = std::get<State>(states);
        currentState = &state;
        return state;
    }

    void backToPrevious()
    {
        if (not previousState.empty())
        {
            currentState = previousState.back();
            previousState.pop_back();
        }
        else
        {
            /* LOG TO FIX*/  LOG_ERROR << ("No previous state!");
        }
    }

    template <typename Event>
    void handle(const Event& event)
    {
        handleBy(event, *this);
    }

    template <typename Event, typename Machine>
    void handleBy(const Event& event, Machine& machine)
    {
        //#ifdef NOREALTIME_LOG_ENABLED
        /* LOG TO FIX*/  LOG_ERROR << ("Handle event : " + typeName<Event>());
        //#endif
        auto passEventToState = [&machine, &event](auto statePtr)
        {
            auto action = statePtr->handle(event);
            action.execute(machine, *statePtr, event);
        };
        std::visit(passEventToState, currentState);
    }

    template <class... T>
    bool isCurrentStateOfType()
    {
        return (std::holds_alternative<T*>(currentState) or ...);
    }

    template <class... T>
    bool isPreviousStateOfType()
    {
        return (std::holds_alternative<T*>(previousState) or ...);
    }

    std::tuple<States...> states;
    std::list<std::variant<States*...>> previousState{&std::get<0>(states)};
    std::variant<States*...> currentState{&std::get<0>(states)};
};
}  // namespace StateMachine
}  // namespace Utils
