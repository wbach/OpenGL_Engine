#include "RotateStateBase.h"

#include <Logger/Log.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
namespace Components
{
RotateStateBase::RotateStateBase(FsmContext &context)
    : context_{context}
{
}
void RotateStateBase::onEnter(const RotateLeftEvent &event)
{
    DEBUG_LOG("onEnter(const RotateLeftEvent& )");
    context_.rotationSpeed  = fabsf(event.speed);
    context_.rotateToTarget = false;
}

void RotateStateBase::onEnter(const RotateRightEvent &event)
{
    DEBUG_LOG("onEnter(const RotateRightEvent& )");
    context_.rotationSpeed  = -fabsf(event.speed);
    context_.rotateToTarget = false;
}

void RotateStateBase::onEnter(const RotateTargetEvent &event)
{
    if (not context_.rotateToTarget or context_.targetRotation != event.target)
    {
        //DEBUG_LOG("onEnter(const RotateTargetEvent& )");
        //DEBUG_LOG(std::to_string(context_.targetRotation) + " | " + std::to_string(event.target));

        context_.startRotation          = context_.rigidbody.GetRotation();
        context_.targetRotation         = event.target;
        context_.rotationSpeed          = event.speed;
        context_.rotateToTarget         = true;
        context_.rotateToTargetProgress = 0.f;
    }
}

void RotateStateBase::update(const AttackEvent &)
{
    //DEBUG_LOG("update(const AttackEvent &)");
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void RotateStateBase::update(const EndAttackEvent &)
{
    //DEBUG_LOG("update(const EndAttackEvent &)");
    context_.attackFsm.handle(AttackFsmEvents::End{});
    context_.multiAnimations = false;
}

void RotateStateBase::update(float deltaTime)
{
    if (not context_.rotateToTarget)
    {
        auto rotation = context_.rigidbody.GetRotation() *
                        glm::angleAxis(glm::radians(context_.rotationSpeed * deltaTime), glm::vec3(0.f, 1.f, 0.f));
        context_.rigidbody.SetRotation(rotation);
    }
    else
    {

        if (context_.rotateToTargetProgress < 1.f)
        {
            context_.rotateToTargetProgress += (context_.rotationSpeed * deltaTime);
            context_.rotateToTargetProgress = glm::clamp(context_.rotateToTargetProgress, 0.f, 1.f);

            auto newRotation =
                glm::slerp(context_.startRotation, context_.targetRotation, context_.rotateToTargetProgress);

            context_.rigidbody.SetRotation(newRotation);
        }
    }
}

}  // namespace Components
}  // namespace GameEngine
