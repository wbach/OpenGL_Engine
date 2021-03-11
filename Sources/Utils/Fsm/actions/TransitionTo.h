#pragma once
#include <Logger/Log.h>

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
        leave(prevState, event);

        TargetState& newState = machine.template transitionTo<TargetState>();

        DEBUG_LOG("prevState : " + typeid(prevState).name() + " newState : " + typeid(newState).name() +
                  " Event : " + typeid(event).name());

        enter(newState);
        enter(newState, event);
        enter(newState, prevState, event);
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
