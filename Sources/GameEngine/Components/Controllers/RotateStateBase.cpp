#include "RotateStateBase.h"
#include "CharacterControllerEvents.h"
#include <Logger/Log.h>

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

void RotateStateBase::update(float deltaTime)
{
    auto rotation = context_.rigidbody.GetRotation() *
                    glm::angleAxis(glm::radians(context_.rotationSpeed * deltaTime), glm::vec3(0.f, 1.f, 0.f));
    context_.rigidbody.SetRotation(rotation);
}

}  // namespace Components
}  // namespace GameEngine
