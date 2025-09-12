#include "JumpState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "CharacterControllerCommonDefs.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Physics/CollisionContactInfo.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const float DEFAULT_JUMP_ATTEMPT_TIMER_VALUE = 0.25f;
}

JumpState::JumpState(FsmContext &context)
    : BaseState{context}
{
}

void JumpState::onEnter(const JumpEvent &event)
{
    auto velocity = context_.rigidbody.GetVelocity();
    velocity.y += event.power;
    context_.rigidbody.SetVelocity(velocity);

    animName = context_.animClipNames.disarmed.jump;

    if (context_.isOnAir)
    {
        setAnim();
    }
    else
    {
        jumpAttemptTimer = DEFAULT_JUMP_ATTEMPT_TIMER_VALUE;
    }
}

void JumpState::update(const JumpConfirmEvent &)
{
    if (jumpAttemptTimer)
    {
        setAnim();
        jumpAttemptTimer.reset();
    }
}

void JumpState::update(float deltaTime)
{
    if (jumpAttemptTimer)
    {
        jumpAttemptTimer = jumpAttemptTimer.value() - deltaTime;
        if (jumpAttemptTimer <= 0.f)
        {
            context_.characterController.pushEventToFrontQueue(GroundDetectionEvent{});
            context_.isOnAir = false;
            jumpAttemptTimer.reset();
        }
    }
}

void JumpState::onLeave(const GroundDetectionEvent &)
{
    flushEvents();
}

void JumpState::setAnim()
{
    if (not animName.empty())
    {
        const auto &currentClips = context_.animator.getCurrentAnimationName();

        auto iter = std::find(currentClips.begin(), currentClips.end(), animName);

        if (iter == currentClips.end())
        {
            context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                              std::nullopt);
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
