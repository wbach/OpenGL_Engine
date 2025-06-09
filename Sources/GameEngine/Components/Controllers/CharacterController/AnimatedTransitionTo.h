#pragma once
#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
class AnimatedTransitionState;

template <typename TargetState>
class AnimatedTransitionTo
{
public:
    template <typename Machine, typename State, typename Event>
    void execute(Machine& machine, State& prevState, const Event& event)
    {
        if (not transitionCondition(prevState, event))
        {
            //#ifdef NOREALTIME_LOG_ENABLED
            DEBUG_LOG("transitionConditions from " + typeName<State>() + " to " + typeName<TargetState>() +
                      " are not met, return");
            //#endif
            return;
        }

        //#ifdef NOREALTIME_LOG_ENABLED
        DEBUG_LOG("PrevState : " + typeName<State>());
        DEBUG_LOG("Entering : " + typeName<TargetState>());
        //#endif

        leave(prevState);
        leave(prevState, event);

        AnimatedTransitionState& newState = machine.template transitionTo<AnimatedTransitionState>();

        TargetState& targetState = std::get<TargetState>(machine.template states);
        enter(newState, prevState, targetState);
        enter(newState, event);
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


    template <typename NewState, typename PrevState, typename Target>
    auto enter(NewState& newState, PrevState& prevState, Target& targetState) -> decltype(newState.onEnter(prevState, targetState))
    {
        return newState.onEnter(prevState, targetState);
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

}  // namespace Components
}  // namespace GameEngine

