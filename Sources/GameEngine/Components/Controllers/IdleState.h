#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class RotateState;
class JumpState;
class AttackState;

class IdleState : public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<MoveState>>,
                      Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<MoveState>>,
                      Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                      Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                      Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                      Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    IdleState(FsmContext& context)
        : context_{context}
    {
    }

    void onEnter()
    {
        DEBUG_LOG("onEnter()");
        if (not context_.idleAnimationName.empty())
        {
            context_.animator.ChangeAnimation(context_.idleAnimationName, Animator::AnimationChangeType::smooth,
                                              PlayDirection::forward, std::nullopt);
        }
    }

    void update(float)
    {
    }

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
