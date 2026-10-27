#pragma once
#include <optional>
#include <typeindex>

#include "CharacterControllerCommonDefs.h"
#include "Fsm/actions/BackToPreviousState.h"
#include "GameEngine/Components/Controllers/CharacterController/States/BaseState.h"
#include "IdleStateBase.h"
#include "StateConcepts.h"
#include "Types.h"
namespace GameEngine
{
namespace Components
{
class HurtState : public BaseState,
                  public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<FallingState>>,
                      Utils::StateMachine::On<AnimationPlayEndEvent, Utils::StateMachine::BackToPreviousState>,
                      Utils::StateMachine::On<DeathEvent, Utils::StateMachine::Queue>>
{
public:
    HurtState(FsmContext&);

    bool entryCondition() const;

    template <typename PrevState, typename Event>
    requires IsCrouchArmedChange<PrevState>
    void onEnter(PrevState& state, const Event&)
    {
        setTypedState<ArmedChangeStateBase, Crouch>();
        setArmedChangeInCrouchRelatedClip();
    }

    template <typename PrevState, typename Event>
    requires IsArmedChange<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setTypedState<ArmedChangeStateBase>();
        setArmedChangeRelatedClip();
    }

    template <typename PrevState, typename Event>
    requires IsCrouchDisarmed<PrevState>
    void onEnter(PrevState& state, const Event&)
    {
        setTypedState<Crouch, Disarmed>();
        setCrouchDisarmedRelatedClip();
    }

    template <typename PrevState, typename Event>
    requires IsCrouchArmed<PrevState>
    void onEnter(PrevState& state, const Event&)
    {
        setTypedState<Crouch, Armed>();
        setCrouchArmedRelatedClip();
    }

    template <typename PrevState, typename Event>
    requires IsDisarmed<PrevState>
    void onEnter(PrevState& state, const Event&)
    {
        setTypedState<Disarmed>();
        setDisarmedRelatedClip();
    }

    template <typename PrevState, typename Event>
    requires IsArmed<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setTypedState<Armed>();
        setArmedRelatedClip();
    }
    void onLeave();

public:
    template <typename... StateType>
    bool isTypedState() const
    {
        static_assert(sizeof...(StateType) > 0);

        return (type.contains(std::type_index(typeid(StateType))) && ...);
    }

private:
    template <typename... StateType>
    void setTypedState()
    {
        static_assert(sizeof...(StateType) > 0);

        type.clear();
        (type.emplace(typeid(StateType)), ...);
    }

    void setArmedRelatedClip();
    void setDisarmedRelatedClip();
    void setCrouchArmedRelatedClip();
    void setCrouchDisarmedRelatedClip();
    void setArmedChangeRelatedClip();
    void setArmedChangeInCrouchRelatedClip();
    void ChangeAnimationAndSubForEnd(const std::string&);

private:
    std::set<std::type_index> type;
    std::optional<IdType> animationSubId;
};
}  // namespace Components
}  // namespace GameEngine
