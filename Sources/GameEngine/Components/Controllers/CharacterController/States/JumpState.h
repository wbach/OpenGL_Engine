#pragma once
#include "CharacterControllerCommonDefs.h"
#include "JumpStateBase.h"

namespace GameEngine
{
namespace Components
{
class JumpState : public JumpStateBase,
                  public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Queue>,
                      Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<DisarmedFallingState>>,
                      Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                      Utils::StateMachine::On<GroundDetectionEvent, Utils::StateMachine::BackToPreviousState>>
{
public:
    JumpState(FsmContext&);
};
}  // namespace Components
}  // namespace GameEngine
