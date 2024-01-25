#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
class AimState;
class ArmedIdleState;
class DisarmedIdleState;

class DrawArrowState : public Utils::StateMachine::Will<
                           Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                           Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
                           Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimState>>,
                           Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>>
{
public:
    DrawArrowState(FsmContext&);
    void onEnter(const DrawArrowEvent&);
    void update(float)
    {
    }

private:
    void setAnim();

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
