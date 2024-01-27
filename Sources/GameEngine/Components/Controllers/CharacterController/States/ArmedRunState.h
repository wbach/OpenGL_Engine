#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class JumpState;
class MoveJumpState;
class DeathState;
class DisarmedRunState;
class ArmedIdleState;
class ArmedWalkState;
class ArmedSprintState;
class ArmedRunAndRotateState;
class DrawArrowRunState;

class ArmedRunState
    : public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          // Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponChangeEndEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
          Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowRunState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    ArmedRunState(FsmContext& context);

    using MoveStateBase::onEnter;
    using MoveStateBase::update;

    void onEnter(const WeaponStateEvent&);

    void onEnter(DisarmedRunState&, const DrawArrowEvent&);
    void onEnter(const DrawArrowEvent&);
    void update(const AimStopEvent&);
    void onLeave();

private:
    std::optional<IdType> drawArrowEndSub_;
};
}  // namespace Components
}  // namespace GameEngine
