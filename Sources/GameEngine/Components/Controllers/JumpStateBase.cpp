#include "JumpStateBase.h"

namespace GameEngine
{
namespace Components
{
JumpStateBase::JumpStateBase(FsmContext &context, std::function<void()> endCallback)
    : context_{context}
    , endCallback_{endCallback}
{
}
void JumpStateBase::onEnter(const JumpEvent &event)
{
    DEBUG_LOG("onEnter()");
    if (not context_.jumpAnimationName.empty())
    {
        context_.animator.ChangeAnimation(
            context_.jumpAnimationName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }

    auto velocity = context_.rigidbody.GetVelocity();
    velocity.y += event.power;
    context_.rigidbody.SetVelocity(velocity);
}

void JumpStateBase::onEnter(const AttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void JumpStateBase::onEnter(const EndAttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::End{});
}
void JumpStateBase::update(float)
{
    if (isGrounded())
    {
        if (endCallback_)
            endCallback_();
    }
}

bool JumpStateBase::isGrounded()
{
    const auto &position = context_.gameObject.GetWorldTransform().GetPosition();
    auto hitTest         = context_.physicsApi.RayTest(position, vec3(position.x, -10000.f, position.z));

    if (hitTest)
    {
        if (glm::length(position - hitTest->pointWorld) < .1f)
        {
            return true;
        }
    }
    return false;

    // https://www.immersivelimit.com/tutorials/simple-character-controller-for-unity

    // float capsuleRadius = 1.f;
    // float capsuleHeight = 1.5f;

    // float capsuleHeight = glm::max(capsuleRadius * 2.f, capsuleHeight);
    // vec3 capsuleBottom  = transform.TransformPoint(capsuleCollider.center - VECTOR_UP * capsuleHeight / 2.f);
    // float radius        = transform.TransformVector(capsuleCollider.radius, 0f, 0f).magnitude;

    // Ray ray = new Ray(capsuleBottom + transform.up * .01f, -transform.up);
    // RaycastHit hit;
    // if (Physics.Raycast(ray, out hit, radius * 5f))
    //{
    //    float normalAngle = glm::angle(hit.normal, transform.up);
    //    if (normalAngle < slopeLimit)
    //    {
    //        float maxDist = radius / Mathf.Cos(Mathf.Deg2Rad * normalAngle) - radius + .02f;
    //        if (hit.distance < maxDist)
    //            return true;
    //    }
    //}
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

}  // namespace Components
}  // namespace GameEngine
