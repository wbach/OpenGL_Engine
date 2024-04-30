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
#ifdef NOREALTIME_LOG_ENABLED
            DEBUG_LOG("transitionConditions from " + typeName<State>() + " to " + typeName<TargetState>() + " are not met, return");
#endif
            return;
        }

#ifdef NOREALTIME_LOG_ENABLED
        DEBUG_LOG("PrevState : " + typeName<State>());
        DEBUG_LOG("Entering : " + typeName<TargetState>());
#endif

        DEBUG_LOG("leave(prevState);");
        leave(prevState);
        DEBUG_LOG("leave(prevState, event);");
        leave(prevState, event);

        TargetState& newState = machine.template transitionTo<TargetState>();

        DEBUG_LOG("enter(newState);");
        enter(newState);
        DEBUG_LOG("enter(newState,event);");
        enter(newState, event);
        DEBUG_LOG("enter(newState,prevState,event);");
        enter(newState, prevState, event);
        DEBUG_LOG("post(newState);");
        post(newState);
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

    template <typename State>
    auto enter(State& state) -> decltype(state.onEnter())
    {
        return state.onEnter();
    }

    template <typename State, typename Event>
    auto enter(State& state, const Event& event) -> decltype(state.onEnter(event))
    {
        return state.onEnter(event);
    }

    template <typename NewState, typename PrevState, typename Event>
    auto enter(NewState& newState, PrevState& prevState, const Event& event)
        -> decltype(newState.onEnter(prevState, event))
    {
        return newState.onEnter(prevState, event);
    }

    template <typename... Args>
    void post(Args&...)
    {
    }

    template <typename State>
    auto post(State& state) -> decltype(state.postEnter())
    {
        return state.postEnter();
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
