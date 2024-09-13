#include "BaseState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine::Components
{
float DEFAULT_FALLING_DETECT_THRESHOLD = -2.2f;

BaseState::BaseState(FsmContext &context, float fallingDetectThreshold)
    : context_{context}
    , fallingDetectThreshold{fallingDetectThreshold}
{
}
void BaseState::update(const TriggerJumpEvent &event)
{
    auto velocity = context_.rigidbody.GetVelocity();
    velocity.y += event.power;
    context_.rigidbody.SetVelocity(velocity);

    // To do cancel subscribtion?
    context_.physicsApi.subscribeForCollisionExit(
        context_.rigidbody.GetId(), [&]() { context_.characterController.pushEventToQueue(JumpEvent{event.power}); });
}

void BaseState::update(float)
{
    //DEBUG_LOG("context_.rigidbody.GetVelocity().y=" + std::to_string(context_.rigidbody.GetVelocity().y));
    if (context_.rigidbody.GetVelocity().y < fallingDetectThreshold)
    {
        //DEBUG_LOG("Trigger");
        //context_.characterController.pushEventToFrontQueue(StartFallingEvent{});
    }
}
}  // namespace GameEngine::Components
