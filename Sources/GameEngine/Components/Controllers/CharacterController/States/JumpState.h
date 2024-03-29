#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "JumpStateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedIdleState;
class MoveJumpState;
class DeathState;

class JumpState : public JumpStateBase,
                  public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<JumpState>>,
                      Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<JumpState>>,
                      Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                      Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                      Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                      Utils::StateMachine::On<EndJumpEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>>

{
public:
    JumpState(FsmContext& context, std::function<void()> endCallback)
        : JumpStateBase(context, std::nullopt, endCallback)
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
