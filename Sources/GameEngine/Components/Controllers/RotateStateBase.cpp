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
    context_.rotationSpeed = fabsf(event.speed);
}

void RotateStateBase::onEnter(const RotateRightEvent &event)
{
    DEBUG_LOG("onEnter(const RotateRightEvent& )");
    context_.rotationSpeed = -fabsf(event.speed);
}

void RotateStateBase::onEnter(const RotateTargetEvent &event)
{
    //        DEBUG_LOG("onEnter(const RotateTargetEvent& )");

    //        progress_ += (deltaTime / rotateSpeed_);

    //        if (progress_ > 1.f)
    //        {
    //            progress_ = 1.f;
    //            toRemove_ = true;
    //        }

    //        auto newRotation = glm::slerp(startedRotation_, rotateTarget_, progress_);
    //        rigidbody_->SetRotation(newRotation);
}

void RotateStateBase::update(const AttackEvent &)
{
    DEBUG_LOG("update(const AttackEvent &)");
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void RotateStateBase::update(const EndAttackEvent &)
{
    DEBUG_LOG("update(const EndAttackEvent &)");
    context_.attackFsm.handle(AttackFsmEvents::End{});
}

void RotateStateBase::update(float deltaTime)
{
    auto rotation = context_.rigidbody.GetRotation() *
                    glm::angleAxis(glm::radians(context_.rotationSpeed * deltaTime), glm::vec3(0.f, 1.f, 0.f));
    context_.rigidbody.SetRotation(rotation);
}

}  // namespace Components
}  // namespace GameEngine
