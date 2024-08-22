#pragma once
#include <Logger/Log.h>

#include <type_traits>
#include <variant>

namespace Utils
{
namespace StateMachine
{
class BackToPreviousState
{
public:
    template <typename Machine, typename State, typename Event>
    void execute(Machine& machine, State& prevState, const Event& event)
    {
#ifdef NOREALTIME_LOG_ENABLED
        DEBUG_LOG("PrevState : " + typeName<State>());
        std::visit([](const auto& state) { DEBUG_LOG("Entering : " + typeName(state)); }, machine.previousState);
#endif

        leave(prevState);
        leave(prevState, event);

        machine.backToPrevious();

        std::visit(
            [&](const auto& state)
            {
                auto& newState = *state;

                enter(newState);
                enter(newState, event);
                enter(newState, prevState);
                enter(newState, prevState, event);
                post(newState);
            },
            machine.currentState);
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

    template <typename NewState, typename PrevState>
    auto enter(NewState& newState, PrevState& prevState) -> decltype(newState.onEnter(prevState))
    {
        return newState.onEnter(prevState);
    }

    template <typename NewState, typename PrevState, typename Event>
    auto enter(NewState& newState, PrevState& prevState, const Event& event) -> decltype(newState.onEnter(prevState, event))
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
