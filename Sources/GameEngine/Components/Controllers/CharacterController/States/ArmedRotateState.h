#pragma once
#include <Logger/Log.h>
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class ArmedIdleState;
class ArmedRunAndRotateState;
class ArmedWalkAndRotateState;
class ArmedSprintAndRotateState;
class JumpState;
class DeathState;
class DisarmedRotateState;
class DrawArrowRotateState;
class DisarmedRunState;

class ArmedRotateState
    : public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponChangeEndEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<ArmedSprintAndRotateState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedRotateState(FsmContext& context);

    using RotateStateBase::onEnter;
    using RotateStateBase::update;

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
