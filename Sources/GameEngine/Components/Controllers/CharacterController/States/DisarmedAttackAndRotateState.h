#pragma once
#include "../CharacterControllerEvents.h"
#include "AttackStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "Fsm/actions/Queue.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedAttackAndRotateState
    : public AttackStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::RemoveFromQueue<DrawArrowEvent>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<DisarmedAttackState>>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>>
{
public:
    DisarmedAttackAndRotateState(FsmContext&);

    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void onEnter(const EndMoveLeftEvent&);
    void onEnter(const EndMoveRightEvent&);

    using RotateStateBase::onEnter;
    using RotateStateBase::update;

    using AttackStateBase::onEnter;
    using AttackStateBase::onLeave;
    using AttackStateBase::update;

    void update(float);

    using MaybeAttackStates = Utils::StateMachine::Maybe<Utils::StateMachine::Update,
                                                         Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>,
                                                         Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkAndRotateState>>;

    MaybeAttackStates handle(const ChangeAnimEvent&);

    using Will::handle;
};
}  // namespace Components
}  // namespace GameEngine
