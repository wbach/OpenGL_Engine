#include "JumpStateBase.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Physics/CollisionContactInfo.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
namespace Components
{
JumpStateBase::JumpStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , groundChecker{nullptr}
{
    createGroundChecker();
}
void JumpStateBase::onEnter(const JumpEvent &event)
{
    DEBUG_LOG("OnEnter");
    getRigidbodyIdsWhenReady();

    if (not context_.animClipNames.disarmed.jump.empty())
    {
        context_.animator.ChangeAnimation(context_.animClipNames.disarmed.jump, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_);
    }

    auto velocity = context_.rigidbody.GetVelocity();
    velocity.y += event.power;
    context_.rigidbody.SetVelocity(velocity);
    lastL = -1.f;
}

void JumpStateBase::update(float)
{
    subscribeForGroundCollisionWhenIsOnAir();
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

void JumpStateBase::subscribeForGroundCollisionWhenIsOnAir()
{
    if (collisionSubId)
        return;

    const auto &position = context_.gameObject.GetWorldTransform().GetPosition();
    auto hitTest         = context_.physicsApi.RayTest(position, vec3(position.x, -10000.f, position.z));

    if (hitTest)
    {
        auto l = glm::length(position - hitTest->pointWorld);
        DEBUG_LOG("L=" + std::to_string(l));

        if ((lastL > l or compare(lastL, l)) or
            (not collisionSubId and l > collisionSphereRadius + std::numeric_limits<float>::epsilon()))
        {
            subscribeForGroundCollision();
        }
        lastL = l;
    }
}
void JumpStateBase::getRigidbodyIdsWhenReady()
{
    if (not groundCheckerRigidbodyId)
    {
        if (auto r = groundChecker->GetComponent<Rigidbody>())
        {
            groundCheckerRigidbodyId = r->GetId();
        }
    }
    if (not playerRigidbodyId)
    {
        if (auto r = context_.gameObject.GetComponent<Rigidbody>())
        {
            playerRigidbodyId = r->GetId();
        }
    }
}
void JumpStateBase::createGroundChecker()
{
    if (not groundChecker)
    {
        auto gc = context_.gameObject.CreateChild(context_.gameObject.GetName() + "_groundChecker");
        gc->AddComponent<SphereShape>().SetRadius(collisionSphereRadius);
        gc->AddComponent<Rigidbody>().SetIsStatic(true).SetNoContactResponse(true);
        groundChecker = gc.get();
        context_.gameObject.AddChild(std::move(gc));
    }
}
void JumpStateBase::subscribeForGroundCollision()
{
    if (groundCheckerRigidbodyId and playerRigidbodyId)
    {
        DEBUG_LOG("setCollisionCallback");
        collisionSubId = context_.physicsApi.setCollisionCallback(
            groundCheckerRigidbodyId,
            [&](const auto &collisionInfo)
            {
                if (collisionInfo.rigidbodyId1 == *playerRigidbodyId or collisionInfo.rigidbodyId2 == *playerRigidbodyId)
                    return;

                if (collisionSubId)
                {
                    DEBUG_LOG("GroundChcker collision detect");
                    context_.characterController.pushEventToQueue(EndJumpEvent{});
                    unsubscribeCollisionCallback();
                }
            });
    }
}
void JumpStateBase::unsubscribeCollisionCallback()
{
    context_.physicsApi.celarCollisionCallback(collisionSubId);
    collisionSubId.reset();
}
}  // namespace Components
}  // namespace GameEngine
