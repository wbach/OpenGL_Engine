#pragma once
/*
https://sii.pl/blog/implementing-a-state-machine-in-c17/
https://github.com/AdamsPL/state-machine
*/
#include <deque>
#include <optional>
#include <string>
#include <tuple>
#include <variant>

#include "Utils/Logger/Log.h"
#include "Utils/Logger/TypeName.h"

namespace Utils
{
namespace StateMachine
{
template <typename... States>
class Fsm
{
public:
    using StateHandle = std::variant<States*...>;

    Fsm() = default;

    Fsm(States... states)
        : states(std::move(states)...)
    {
    }

    template <typename State>
    State& transitionTo()
    {
        rememberPreviousState();

        auto& state = std::get<State>(states);
        setCurrentState(&state);
        return state;
    }

    void backToPrevious()
    {
        if (previousState.empty())
        {
            LOG_ERROR << "No previous state!";
            return;
        }

        setCurrentState(previousState.back());
        previousState.pop_back();
    }

    void saveCurrentState()
    {
        LOG_DEBUG << "Save state : " << getCurrentStateName();
        savedState = currentState;
    }

    void savePreviousState()
    {
        if (previousState.empty())
        {
            LOG_ERROR << "No previous state!";
            return;
        }

        auto prevState = previousState.back();
        LOG_DEBUG << "Save state : " << getStateName(prevState);
        savedState = prevState;
    }

    bool returnToSavedState()
    {
        if (not savedState)
        {
            LOG_ERROR << "No saved state!";
            return false;
        }

        rememberPreviousState();
        setCurrentState(*savedState);
        savedState.reset();
        return true;
    }

    template <typename Event>
    void handle(const Event& event)
    {
        handleBy(event, *this);
    }

    template <typename Event, typename Machine>
    void handleBy(const Event& event, Machine& machine)
    {
        // #ifdef NOREALTIME_LOG_ENABLED
        LOG_DEBUG << "Handle event : " << typeName<Event>();
        // #endif
        auto passEventToState = [&machine, &event](auto statePtr)
        {
            auto action = statePtr->handle(event);
            action.execute(machine, *statePtr, event);
        };
        std::visit(passEventToState, currentState);
    }

    template <class... T>
    bool isCurrentStateOfType() const
    {
        return (std::holds_alternative<T*>(currentState) or ...);
    }

    template <class... T>
    bool isPreviousStateOfType() const
    {
        return (std::holds_alternative<T*>(previousState) or ...);
    }

    std::string getCurrentStateName() const
    {
        return getStateName(currentState);
    }

private:
    std::string getStateName(const StateHandle& state) const
    {
        auto passEventToState = [](auto* statePtr) { return typeName<std::decay_t<decltype(*statePtr)>>(); };
        return std::visit(passEventToState, state);
    }

    void rememberPreviousState()
    {
        previousState.push_back(currentState);
        if (previousState.size() > 10)
        {
            previousState.pop_front();
        }
    }

    void setCurrentState(const StateHandle& state)
    {
        currentState = state;
        LOG_DEBUG << "Current state changed. New state: " << getCurrentStateName();
    }

public:
    std::tuple<States...> states;

    StateHandle currentState{&std::get<0>(states)};
    std::optional<StateHandle> savedState;
    std::deque<StateHandle> previousState{&std::get<0>(states)};
};
}  // namespace StateMachine
}  // namespace Utils
