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
class RecoilRotateState;

class RecoilState : public RecoilStateBase,
                    public Utils::StateMachine::Will<
                        Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                        Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
                        Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
                        Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
                        Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
                        Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>>
{
public:
    RecoilState(FsmContext&);
};
}  // namespace Components
}  // namespace GameEngine
