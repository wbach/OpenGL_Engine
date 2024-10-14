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
const float DEFAULT_JUMP_ATTEMPT_TIMER_VALUE = 0.2f;
}

JumpState::JumpState(FsmContext &context)
    : BaseState{context}
{
}

void JumpState::onEnter(const JumpEvent &event)
{
    auto velocity = context_.rigidbody.GetVelocity();
    velocity.y += event.jumpPower;
    context_.rigidbody.SetVelocity(velocity);

    jumpAttemptTimer = DEFAULT_JUMP_ATTEMPT_TIMER_VALUE;
}

void JumpState::update(const JumpConfirmEvent &)
{
    jumpAttemptTimer.reset();
    setAnim();
}

void JumpState::update(float deltaTime)
{
    if (jumpAttemptTimer)
    {
        jumpAttemptTimer = jumpAttemptTimer.value() - deltaTime;
        if (jumpAttemptTimer <= 0.f)
        {
            DEBUG_LOG("JumpTriger timeout");
            context_.characterController.pushEventToFrontQueue(GroundDetectionEvent{});
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
    if (not context_.animClipNames.disarmed.jump.empty())
    {
        context_.animator.ChangeAnimation(context_.animClipNames.disarmed.jump, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, std::nullopt);
    }
}
}  // namespace Components
}  // namespace GameEngine
