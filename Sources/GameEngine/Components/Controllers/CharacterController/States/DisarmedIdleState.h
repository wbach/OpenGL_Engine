#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedRunState;
class DisarmedRotateState;
class DisarmedWalkState;
class DisarmedSprintState;
class DisarmedCrouchState;
class JumpState;
class DeathState;
class IdleArmedChangeState;

class DisarmedIdleState
    : public IdleStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          /* Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
           Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,*/
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
          Utils::StateMachine::On<RunLeftEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
          Utils::StateMachine::On<RunRightEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<WalkLeftEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<WalkRightEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<DisarmedSprintState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    DisarmedIdleState(FsmContext& context);

    using IdleStateBase::onEnter;
};
}  // namespace Components
}  // namespace GameEngine
