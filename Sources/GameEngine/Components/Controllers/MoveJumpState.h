#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class JumpState;

class MoveJumpState : public Utils::StateMachine::Will<
                          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                          Utils::StateMachine::On<EndMoveEvent, Utils::StateMachine::TransitionTo<JumpState>>,
                          Utils::StateMachine::On<EndJumpEvent, Utils::StateMachine::TransitionTo<MoveState>>>
{
public:
    MoveJumpState(FsmContext& context, std::function<void()> endCallback)
        : context_{context}
        , endCallback_{endCallback}
    {
    }

    void onEnter(const JumpEvent& event)
    {
        DEBUG_LOG("onEnter(JumpEvent)");
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
            if (endCallback_)
                endCallback_();
        }
    }

    bool isGrounded()
    {
        const auto& position = context_.gameObject.GetWorldTransform().GetPosition();
        auto hitTest = context_.physicsApi.RayTest(position, vec3(position.x, -10000.f, position.z));

        if (hitTest)
        {
            if (glm::length(position - hitTest->pointWorld) < .1f)
            {
                return true;
            }
        }
        return false;

        // https://www.immersivelimit.com/tutorials/simple-character-controller-for-unity

        //        float capsuleHeight = Mathf.Max(capsuleCollider.radius * 2f, capsuleCollider.height);
        //        Vector3 capsuleBottom = transform.TransformPoint(capsuleCollider.center - Vector3.up * capsuleHeight /
        //        2f); float radius = transform.TransformVector(capsuleCollider.radius, 0f, 0f).magnitude;

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
        return true;
    }

private:
    FsmContext& context_;
    std::function<void()> endCallback_;
};
}  // namespace Components
}  // namespace GameEngine
