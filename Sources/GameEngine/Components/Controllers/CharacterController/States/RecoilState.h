#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "RecoilStateBase.h"

namespace GameEngine
{
namespace Components
{
class DrawArrowState;
class ArmedIdleState;
class DisarmedIdleState;
class RecoilRunState;
class RecoilWalkState;
class RecoilRotateState;

class RecoilState : public RecoilStateBase,
                    public Utils::StateMachine::Will<
                        Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                        Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<RecoilRunState>>,
                        Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<RecoilRunState>>,
                        Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<RecoilWalkState>>,
                        Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<RecoilWalkState>>,
                        Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
                        Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
                        Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
                        Utils::StateMachine::On<ReloadArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
                        Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>>
{
public:
    RecoilState(FsmContext&);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
};
}  // namespace Components
}  // namespace GameEngine
