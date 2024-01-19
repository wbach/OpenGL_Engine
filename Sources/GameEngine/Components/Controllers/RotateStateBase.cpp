#include "RotateStateBase.h"

#include <Logger/Log.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
namespace Components
{
RotateStateBase::RotateStateBase(FsmContext &context, float rotateSpeed, const std::string &rotateLeftAnim,
                                 const std::string &rotateRightAnim)
    : StateBase{context}
    , rotateLeftAnim_{rotateLeftAnim}
    , rotateRightAnim_{rotateRightAnim}
    , rotateSpeed_{rotateSpeed}
{
}
void RotateStateBase::onEnter(const RotateLeftEvent &e)
{
    update(e);
    setRotateLeftAnim();
}

void RotateStateBase::onEnter(const RotateRightEvent &e)
{
    update(e);
    setRotateRightAnim();
}

void RotateStateBase::onEnter(const RotateTargetEvent &event)
{
    if (not context_.rotateToTarget or context_.targetRotation != event.target)
    {
        context_.startRotation          = context_.rigidbody.GetRotation();
        context_.targetRotation         = event.target;
        context_.rotateToTarget         = true;
        context_.rotateToTargetProgress = 0.f;
    }
}

void RotateStateBase::onEnter(const EndForwardMoveEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::onEnter(const EndBackwardMoveEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::update(const AttackEvent &)
{
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void RotateStateBase::update(const EndAttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::End{});
    context_.multiAnimations = false;
}

void RotateStateBase::update(float deltaTime)
{
    if (not context_.rotateToTarget)
    {
        auto rotation =
            context_.rigidbody.GetRotation() *
            glm::angleAxis(glm::radians(context_.rotateStateData_.rotateSpeed_ * deltaTime), glm::vec3(0.f, 1.f, 0.f));
        context_.rigidbody.SetRotation(rotation);
    }
    else
    {
        if (context_.rotateToTargetProgress < 1.f)
        {
            context_.rotateToTargetProgress += (context_.rotateStateData_.rotateSpeed_ * deltaTime);
            context_.rotateToTargetProgress = glm::clamp(context_.rotateToTargetProgress, 0.f, 1.f);

            auto newRotation =
                glm::slerp(context_.startRotation, context_.targetRotation, context_.rotateToTargetProgress);

            context_.rigidbody.SetRotation(newRotation);
        }
    }
}

void RotateStateBase::update(const RotateLeftEvent &)
{
    context_.rotateStateData_.rotateSpeed_ = fabsf(rotateSpeed_);
    context_.rotateToTarget                = false;
}

void RotateStateBase::update(const RotateRightEvent &)
{
    context_.rotateStateData_.rotateSpeed_ = -fabsf(rotateSpeed_);
    context_.rotateToTarget                = false;
}

void RotateStateBase::update(const RotateTargetEvent &)
{
}

void RotateStateBase::update(const WeaponChangeEndEvent &)
{
    context_.multiAnimations = false;
}

void RotateStateBase::setRotateLeftAnim()
{
    if (not rotateLeftAnim_.empty())
    {
        context_.animator.ChangeAnimation(
            rotateLeftAnim_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void RotateStateBase::setRotateRightAnim()
{
    if (not rotateRightAnim_.empty())
    {
        context_.animator.ChangeAnimation(
            rotateRightAnim_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void RotateStateBase::setCurrentAnim()
{
    if (context_.rotateStateData_.rotateSpeed_ > 0.01f)
    {
        setRotateLeftAnim();
    }
    else if (context_.rotateStateData_.rotateSpeed_ < -0.01f)
    {
        setRotateRightAnim();
    }
}

}  // namespace Components
}  // namespace GameEngine
