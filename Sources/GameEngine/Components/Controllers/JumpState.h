#pragma once
#include <Utils/Fsm/Actions.h>
#include <Utils/Fsm/Fsm.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"


namespace GameEngine
{
namespace Components
{
class MoveState;
class RotateState;
class AttackState;
class IdleState;

class JumpState : public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<EndJumpEvent, Utils::StateMachine::TransitionTo<IdleState>>>
{
public:
    JumpState(FsmContext& context)
        : context_{context}
    {
    }

    void onEnter(const JumpEvent& event)
    {
        DEBUG_LOG("onEnter()");
        if (not context_.jumpAnimationName.empty())
        {
            context_.animator.ChangeAnimation(context_.jumpAnimationName, Animator::AnimationChangeType::smooth,
                                              PlayDirection::forward, std::nullopt);
        }

        auto velocity = context_.rigidbody.GetVelocity();
        velocity.y += event.power;
        context_.rigidbody.SetVelocity(velocity);
    }

    void update(float)
    {
        if (isGrounded())
        {

        }
    }

    bool isGrounded()
    {
        //https://www.immersivelimit.com/tutorials/simple-character-controller-for-unity

//        float capsuleHeight = Mathf.Max(capsuleCollider.radius * 2f, capsuleCollider.height);
//        Vector3 capsuleBottom = transform.TransformPoint(capsuleCollider.center - Vector3.up * capsuleHeight / 2f);
//        float radius = transform.TransformVector(capsuleCollider.radius, 0f, 0f).magnitude;

//        Ray ray = new Ray(capsuleBottom + transform.up * .01f, -transform.up);
//        RaycastHit hit;
//        if (Physics.Raycast(ray, out hit, radius * 5f))
//        {
//            float normalAngle =  glm::angle(hit.normal, transform.up);
//            if (normalAngle < slopeLimit)
//            {
//                float maxDist = radius / Mathf.Cos(Mathf.Deg2Rad * normalAngle) - radius + .02f;
//                if (hit.distance < maxDist)
//                    return true;
//            }
//        }
        return false;
    }

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
