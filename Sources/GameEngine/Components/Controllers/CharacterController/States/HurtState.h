#pragma once
#include <optional>

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
class HurtState
    : public BaseState,
      public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<AnimationPlayEndEvent, Utils::StateMachine::BackToPreviousState>,
                                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::Queue>>
{
public:
    HurtState(FsmContext&);

    bool entryCondition() const;

    template <typename PrevState, typename Event>
        requires IsCrouchArmedChange<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setArmedChangeInCrouchRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsArmedChange<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setArmedChangeRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsCrouchDisarmed<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setCrouchDisarmedRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsCrouchArmed<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setCrouchArmedRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsDisarmed<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setDisarmedRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsArmed<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setArmedRelatedClip();
    }
    void onLeave();

private:
    void setArmedRelatedClip();
    void setDisarmedRelatedClip();
    void setCrouchArmedRelatedClip();
    void setCrouchDisarmedRelatedClip();
    void setArmedChangeRelatedClip();
    void setArmedChangeInCrouchRelatedClip();
    void ChangeAnimationAndSubForEnd(const std::string&);

private:
    std::optional<IdType> animationSubId;
};
}  // namespace Components
}  // namespace GameEngine
