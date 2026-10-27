#pragma once
#include "BaseState.h"
#include "CharacterControllerCommonDefs.h"
#include "Stand.h"
#include "StateConcepts.h"

namespace GameEngine
{
namespace Components
{
class FallingState
    : public BaseState,
      public Stand,
      public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Queue>,
                                       Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>>
{
public:
    FallingState(FsmContext&);

    template <typename PrevState, typename Event>
        requires IsArmedChange<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setArmedChangeRelatedClip();
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

    template <typename PrevState, typename Event>
    void onEnter(PrevState&, const Event&)
    {
        setDisarmedRelatedClip();
    }

    void onEnter(HurtState&, const StartFallingEvent&);

    void onLeave(const GroundDetectionEvent&);

    using StateAfterLand =
        Utils::StateMachine::Maybe<Utils::StateMachine::BackToPreviousState, Utils::StateMachine::TransitionTo<DisarmedIdleState>,
                                   Utils::StateMachine::TransitionTo<ArmedIdleState>,
                                   Utils::StateMachine::TransitionTo<DeathState>>;

    StateAfterLand handle(const GroundDetectionEvent&);

    using Will::handle;

private:
    void setArmedChangeRelatedClip();
    void setDisarmedRelatedClip();
    void setArmedRelatedClip();
    void changeAnim(const std::string&);
};
}  // namespace Components
}  // namespace GameEngine
