#pragma once
#include "CharacterControllerCommonDefs.h"
#include "FallingStateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedFallingState
    : public FallingStateBase,
      public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>>
{
public:
    DisarmedFallingState(FsmContext&);

    using FallingStateBase::handle;
    using Will::handle;
};
}  // namespace Components
}  // namespace GameEngine
