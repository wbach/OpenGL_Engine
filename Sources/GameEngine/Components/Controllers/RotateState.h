#pragma once
#include <Logger/Log.h>
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class MoveAndRotateState;
class JumpState;
class IdleState;

class RotateState
    : public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<IdleState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    RotateState(FsmContext& context)
        : context_{context}
    {
    }

    void onEnter(const RotateLeftEvent& event)
    {
        DEBUG_LOG("onEnter(const RotateLeftEvent& )");
        context_.rotationSpeed = fabsf(event.speed);
    }
    void onEnter(const RotateRightEvent& event)
    {
        DEBUG_LOG("onEnter(const RotateRightEvent& )");
        context_.rotationSpeed = -fabsf(event.speed);
    }
    void onEnter(const RotateTargetEvent& event)
    {
        DEBUG_LOG("onEnter(const RotateTargetEvent& )");
    }

    void update(float deltaTime)
    {
        auto rotation = context_.rigidbody.GetRotation() *
                        glm::angleAxis(glm::radians(context_.rotationSpeed * deltaTime), glm::vec3(0.f, 1.f, 0.f));
        context_.rigidbody.SetRotation(rotation);
    }

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
