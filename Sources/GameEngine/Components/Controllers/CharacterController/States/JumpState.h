#pragma once
#include "CharacterControllerCommonDefs.h"
#include "JumpStateBase.h"

namespace GameEngine
{
namespace Components
{
class JumpState
    : public JumpStateBase,
      public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Queue>,
                                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                                       Utils::StateMachine::On<EndJumpEvent, Utils::StateMachine::BackToPreviousState>>
{
public:
    JumpState(FsmContext&);
};
}  // namespace Components
}  // namespace GameEngine
