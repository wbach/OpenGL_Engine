#pragma once
#include "CharacterControllerCommonDefs.h"
#include "FallingStateBase.h"
#include "Armed.h"
namespace GameEngine
{
namespace Components
{
class ArmedFallingState
    : public Armed,
      public FallingStateBase,
      public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Queue>,
                                       Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>>
{
public:
    ArmedFallingState(FsmContext&);

    using FallingStateBase::handle;
    using Will::handle;
};
}  // namespace Components
}  // namespace GameEngine
