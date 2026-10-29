#pragma once
#include <Logger/Log.h>
#include <Utils/Logger/TypeName.h>

#include <variant>

namespace Utils
{
namespace StateMachine
{
class BackToSavedState
{
public:
    template <typename Machine, typename State, typename Event>
    void execute(Machine& machine, State& prevState, const Event& event)
    {
        if (not machine.savedState)
        {
            LOG_DEBUG << "SavedState state empty";
            return;
        }

        // #ifdef NOREALTIME_LOG_ENABLED
        LOG_DEBUG << "Prev saved state : " << typeName<State>();
        std::visit([](const auto& state) { LOG_DEBUG << "Entering : " << typeName(state); }, machine.savedState.value());
        // #endif

        leave(prevState);
        leave(prevState, event);
        postLeave(prevState);

        if (not machine.returnToSavedState())
        {
            return;
        }

        LOG_DEBUG << machine.getCurrentStateName();
        std::visit(
            [&](const auto& state)
            {
                auto& newState = *state;
                enter(newState);
                enter(newState, event);
                enter(newState, prevState);
                enter(newState, prevState, event);
                LOG_DEBUG << "";
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
    void postLeave(Args&...)
    {
    }

    template <typename State>
    auto postLeave(State& state) -> decltype(state.onLeaveComplete())
    {
        return state.onLeaveComplete();
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
    bool leaveCondition(Args&...)
    {
        return true;
    }

    template <typename State, typename Event>
    auto leaveCondition(State& state, const Event& event) -> decltype(state.leaveCondition(event))
    {
        return state.leaveCondition(event);
    }
};
}  // namespace StateMachine
}  // namespace Utils
