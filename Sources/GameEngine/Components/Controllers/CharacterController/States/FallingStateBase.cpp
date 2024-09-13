#include "FallingStateBase.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Physics/CollisionContactInfo.h"

namespace GameEngine
{
namespace Components
{
FallingStateBase::FallingStateBase(FsmContext &context, const std::string &animName)
    : BaseState(context)
    , animName{animName}
{
}

void FallingStateBase::onEnter(const StartFallingEvent &)
{
    if (not animName.empty())
    {
        context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward, std::nullopt);
    }

    subscribeForGroundContact();
}

DisarmedFallingState::StateAfterLand FallingStateBase::handle(const EndFallingEvent &)
{
    bool isNotDead{true};
    if (isNotDead)
    {
        return Utils::StateMachine::BackToPreviousState{};
    }
    return Utils::StateMachine::TransitionTo<DeathState>{};
}

void FallingStateBase::update(float)
{
}

void FallingStateBase::subscribeForGroundContact()
{
    collisionSubId = context_.physicsApi.setCollisionCallback(
        context_.rigidbody.GetId(),
        [&](const auto &collisionInfo)
        {
            if (collisionSubId)
            {
                const auto &playerPosition     = context_.gameObject.GetWorldTransform().GetPosition();
                const auto playerPosWithOffset = playerPosition + vec3(0, 0.25f, 0);

                if (context_.rigidbody.GetId() == collisionInfo.rigidbodyId1)
                {
                    if (collisionInfo.pos2.y <= playerPosWithOffset.y)
                    {
                        sendEndJumptEvent();
                    }
                }
                else
                {
                    if (collisionInfo.pos1.y <= playerPosWithOffset.y)
                    {
                        sendEndJumptEvent();
                    }
                }
            }
        });
}

void FallingStateBase::sendEndJumptEvent()
{
    DEBUG_LOG("Ground collision detect, send EndFallingEvent");
    context_.characterController.pushEventToQueue(EndFallingEvent{});
    context_.physicsApi.celarCollisionCallback(collisionSubId);
    collisionSubId.reset();
}

}  // namespace Components
}  // namespace GameEngine
