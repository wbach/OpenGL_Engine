#pragma once
#include "AttackStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "RotateStateBase.h"
#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
class DisarmedAttackAndRunAndRotateState
    : public AttackStateBase,
      public RotateStateBase,
      public RotatingMoveState,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent,
                                  Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRunState>>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>>
{
public:
    DisarmedAttackAndRunAndRotateState(FsmContext&);

    using AttackStateBase::onEnter;
    using AttackStateBase::update;
    using AttackStateBase::onLeave;

    using RotatingMoveState::onEnter;
    using RotatingMoveState::update;
    using RotatingMoveState::onLeave;

    using RotateStateBase::onEnter;
    using RotateStateBase::update;

    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);

    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
