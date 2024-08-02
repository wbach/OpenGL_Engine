#pragma once
#include "CharacterControllerCommonDefs.h"
#include "DrawArrowStateBase.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class DrawArrowCrouchWalkState
    : public DrawArrowStateBase,
      public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<CrouchWalkArmedChangeState>>,
          //          Utils::StateMachine::On<RotateLeftEvent,
          //          Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          //          Utils::StateMachine::On<RotateRightEvent,
          //          Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          //          Utils::StateMachine::On<RotateTargetEvent,
          //          Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimCrouchWalkState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>>
{
public:
    DrawArrowCrouchWalkState(FsmContext&);

    using MoveStateBase::onEnter;
    using MoveStateBase::transitionCondition;
    using MoveStateBase::update;

    void onEnter(ArmedSprintState&, const DrawArrowEvent&);
    void onEnter(ArmedRunState&, const DrawArrowEvent&);
    void onEnter(ArmedWalkState&, const DrawArrowEvent&);

    void onEnter(const DrawArrowEvent&);
    void onEnter(const ReloadArrowEvent&);

    void update(float);

    void onMoveInactivity() override;

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
    void onLeave(const SprintStateChangeEvent&);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
