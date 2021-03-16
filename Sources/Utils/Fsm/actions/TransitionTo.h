#pragma once
#include <Logger/Log.h>

#include <type_traits>

namespace Utils
{
namespace StateMachine
{
template <typename TargetState>
class TransitionTo
{
public:
    template <typename Machine, typename State, typename Event>
    void execute(Machine& machine, State& prevState, const Event& event)
    {
        leave(prevState);
        leave(prevState, event);

        TargetState& newState = machine.template transitionTo<TargetState>();

        enter(newState, prevState, event);
        enter(newState, event);
        enter(newState);
    }

private:
    void leave(...)
    {
    }

    template <typename State, typename Event>
    auto leave(State& state, const Event& event) -> decltype(state.onLeave(event))
    {
        return state.onLeave(event);
    }

    template <typename State>
    auto leave(State& state) -> decltype(state.onLeave())
    {
        return state.onLeave();
    }

    void enter(...)
    {
    }

    template <typename State, typename Event>
    auto enter(State& state, const Event& event) -> decltype(state.onEnter(event))
    {
        return state.onEnter(event);
    }

    template <typename NewState, typename PrevState, typename Event>
    auto enter(NewState& newState, const PrevState& prevState, const Event& event)
        -> decltype(newState.onEnter(prevState, event))
    {
        return newState.onEnter(prevState, event);
    }

    template <typename State>
    auto enter(State& state) -> decltype(state.onEnter())
    {
        return state.onEnter();
    }
};
}  // namespace StateMachine
}  // namespace Utils
