#pragma once
#include "CharacterControllerCommonDefs.h"
#include "JumpStateBase.h"

namespace GameEngine
{
namespace Components
{
class MoveJumpState : public JumpStateBase,
                      public Utils::StateMachine::Will<
                          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<JumpState>>,
                          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<JumpState>>,
                          Utils::StateMachine::On<EndJumpEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>>
{
public:
    MoveJumpState(FsmContext&, std::function<void()>);
};
}  // namespace Components
}  // namespace GameEngine
