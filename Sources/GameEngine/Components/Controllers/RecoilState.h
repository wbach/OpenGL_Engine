#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"

namespace GameEngine
{
namespace Components
{
class DrawArrowState;
class ArmedIdleState;

class RecoilState : public Utils::StateMachine::Will<
                        Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                        Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
                        Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>>

// Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
// Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
// Utils::StateMachine::On<WeaponChangeEndEvent, Utils::StateMachine::Update>,
// Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
// Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>,
// Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>,
// Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>,
// Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
// Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
// Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
// Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
// Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
// Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
// Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
// Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
// Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    RecoilState(FsmContext&);
    void onEnter(const ShootEvent);
    void update(float)
    {
    }

private:
    void setAnim();

private:
    FsmContext& context_;
    std::string animName_;
};
}  // namespace Components
}  // namespace GameEngine
