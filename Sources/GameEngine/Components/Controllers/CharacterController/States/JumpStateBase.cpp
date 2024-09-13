#include "JumpStateBase.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Physics/CollisionContactInfo.h"

namespace GameEngine
{
namespace Components
{
JumpStateBase::JumpStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName)
    : BaseState{context, -10.f}
    , jointGroupName_{jointGroupName}
{
}
void JumpStateBase::onEnter(const JumpEvent &event)
{
    subscribeForGroundCollision();

    if (not context_.animClipNames.disarmed.jump.empty())
    {
        context_.animator.ChangeAnimation(context_.animClipNames.disarmed.jump, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_);
    }
}

void JumpStateBase::onLeave(const EndJumpEvent &)
{
    for (const auto &e : queue)
    {
        context_.characterController.pushEventToQueue(e);
    }
    queue.clear();

    if (collisionSubId)
    {
        unsubscribeCollisionCallback();
    }
}

void JumpStateBase::sendEndJumptEvent()
{
    DEBUG_LOG("Ground collision detect, send EndJumpEvent");
    context_.characterController.pushEventToQueue(EndJumpEvent{});
    unsubscribeCollisionCallback();
}

void JumpStateBase::subscribeForGroundCollision()
{
    collisionSubId = context_.physicsApi.setCollisionCallback(
        context_.rigidbody.GetId(),
        [&](const auto &collisionInfo)
        {
            if (collisionSubId)
            {
                const auto &playerPosition     = context_.gameObject.GetWorldTransform().GetPosition();
                const auto playerPosWithOffset = playerPosition + vec3(0, 0.25f, 0);

                // DEBUG_LOG("Normal : " + std::to_string(collisionInfo.normal));
                if (context_.rigidbody.GetId() == collisionInfo.rigidbodyId1)
                {
                    if (collisionInfo.pos2.y <= playerPosWithOffset.y)
                    {
                        // DEBUG_LOG("collisionInfo.rigidbodyId2=" + std::to_string(collisionInfo.rigidbodyId2));
                        sendEndJumptEvent();
                    }
                }
                else
                {
                    if (collisionInfo.pos1.y <= playerPosWithOffset.y)
                    {
                        // DEBUG_LOG("collisionInfo.rigidbodyId1=" + std::to_string(collisionInfo.rigidbodyId1));
                        sendEndJumptEvent();
                    }
                }
            }
        });
}
void JumpStateBase::unsubscribeCollisionCallback()
{
    context_.physicsApi.celarCollisionCallback(collisionSubId);
    collisionSubId.reset();
}
}  // namespace Components
}  // namespace GameEngine
