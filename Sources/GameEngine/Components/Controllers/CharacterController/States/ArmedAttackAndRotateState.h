#pragma once
#include "AttackStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "Fsm/actions/Queue.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class ArmedAttackAndRotateState
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
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRotateState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
          Utils::StateMachine::On<DodgeDiveEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeForwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeBackwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeLeftEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeRightEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<ArmedAttackState>>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>>
{
public:
    ArmedAttackAndRotateState(FsmContext&);

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
                                                         Utils::StateMachine::TransitionTo<ArmedAttackAndRunAndRotateState>,
                                                         Utils::StateMachine::TransitionTo<ArmedAttackAndWalkAndRotateState>>;

    MaybeAttackStates handle(const ChangeAnimEvent&);

    using Will::handle;

private:
    FsmContext& context;
};
}  // namespace Components
}  // namespace GameEngine
