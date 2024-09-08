#include "BaseState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine::Components
{
BaseState::BaseState(FsmContext &context)
    : context_{context}
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
}  // namespace GameEngine::Components