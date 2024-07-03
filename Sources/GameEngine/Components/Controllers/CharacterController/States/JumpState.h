#pragma once
#include "CharacterControllerCommonDefs.h"
#include "JumpStateBase.h"

namespace GameEngine
{
namespace Components
{
class JumpState : public JumpStateBase,
                  public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<JumpState>>,
                      Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<JumpState>>,
                      Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                      Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                      Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                      Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                      Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                      Utils::StateMachine::On<EndJumpEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>>

{
public:
    JumpState(FsmContext&, std::function<void()>);
};
}  // namespace Components
}  // namespace GameEngine
