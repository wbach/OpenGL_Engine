#pragma once
#include <Types.h>
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "CommonActions.h"
#include "FsmContext.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class RotateState;
class MoveAndRotateState;
class JumpState;
class IdleState;
class MoveJumpState;

class MoveState : public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<EndMoveEvent, Utils::StateMachine::TransitionTo<IdleState>>,
                      Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
                      Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
                      Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
                      Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    MoveState(FsmContext& context)
        : context_{context}
    {
    }

    void onEnter(const EndJumpEvent& event)
    {
        if (context_.moveDirection.z > 0)
        {
            setForwardAnim();
        }
        else
        {
            setBackwardAnim();
        }
    }

    void onEnter(const MoveForwardEvent& event)
    {
        DEBUG_LOG("onEnter(const MoveForwardEvent&)");

        context_.moveDirection = vec3(0.f, 0.f, 1.f);
        context_.moveSpeed     = fabsf(event.moveSpeed);

        setForwardAnim();
    }

    void onEnter(const MoveBackwardEvent& event)
    {
        DEBUG_LOG("onEnter(const MoveBackwardEvent&)");

        context_.moveDirection = vec3(0.f, 0.f, -1.f);
        context_.moveSpeed     = fabsf(event.moveSpeed);

        setBackwardAnim();
    }

    void update(float)
    {
        moveRigidbody(context_);
    }

private:
    void setForwardAnim()
    {
        if (not context_.forwardAnimationName.empty())
        {
            context_.animator.ChangeAnimation(context_.forwardAnimationName, Animator::AnimationChangeType::smooth,
                PlayDirection::forward, std::nullopt);
        }
    }
    void setBackwardAnim()
    {
        if (not context_.backwardAnimationName.empty())
        {
            context_.animator.ChangeAnimation(context_.backwardAnimationName, Animator::AnimationChangeType::smooth,
                PlayDirection::forward, std::nullopt);
        }
        else
        {
            if (not context_.forwardAnimationName.empty())
            {
                context_.animator.ChangeAnimation(context_.forwardAnimationName, Animator::AnimationChangeType::smooth,
                    PlayDirection::backward, std::nullopt);
            }
        }
    }

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
