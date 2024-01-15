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
        if (not transitionCondition(prevState, event))
        {
            return;
        }
        leave(prevState);
        leave(prevState, event);

        // DEBUG_LOG("Entering : " + typeName<TargetState>());

        TargetState& newState = machine.template transitionTo<TargetState>();

        enter(newState, prevState, event);
        enter(newState, event);
        enter(newState);
    }

private:
    template <typename... Args>
    void leave(Args&...)
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

    template <typename... Args>
    void enter(Args&...)
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

    template <typename... Args>
    bool transitionCondition(Args&...)
    {
        return true;
    }

    template <typename State, typename Event>
    auto transitionCondition(State& state, const Event& event) -> decltype(state.transitionCondition(event))
    {
        return state.transitionCondition(event);
    }
};
}  // namespace StateMachine
}  // namespace Utils
