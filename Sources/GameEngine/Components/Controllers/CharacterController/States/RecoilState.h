#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
class DrawArrowState;
class ArmedIdleState;
class DisarmedIdleState;

class RecoilState : public Utils::StateMachine::Will<
                        Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                        Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
                        Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
                        Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>>
{
public:
    RecoilState(FsmContext&);
    void onEnter(const AttackEvent&);
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
