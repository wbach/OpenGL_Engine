#pragma once
#include "CharacterControllerCommonDefs.h"
#include "Disarmed.h"
#include "FallingStateBase.h"
#include "Stand.h"
namespace GameEngine
{
namespace Components
{
class DisarmedFallingState
    : public Stand,
      public Disarmed,
      public FallingStateBase,
      public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Queue>,
                                       Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>>
{
public:
    DisarmedFallingState(FsmContext&);

    using FallingStateBase::handle;
    using Will::handle;
};
}  // namespace Components
}  // namespace GameEngine
