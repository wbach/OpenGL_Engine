#include "RotateStateBase.h"

#include <Logger/Log.h>

#include "../FsmContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
namespace Components
{
RotateStateBase::RotateStateBase(FsmContext &context, const std::optional<std::string> &jointGourpName, float rotateSpeed,
                                 const RotateAnimationClips &rotateAnimationClips, CameraRotationPolicy cameraRotationPolicy)
    : context_{context}
    , jointGroupName_{jointGourpName}
    , rotateAnimationClips_{rotateAnimationClips}
    , rotateSpeed_{rotateSpeed}
    , cameraComponent_{context_.gameObject.GetComponent<ThridPersonCameraComponent>()}
    , cameraRotationPolicy_{cameraRotationPolicy}
{
}

void RotateStateBase::onEnter(const CrouchChangeStateEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::onEnter(const EndAttackEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::onEnter(const EquipEndStateEvent &)
{
    setCurrentAnim();
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

        setCurrentAnim();
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

void RotateStateBase::onEnter(const EndMoveLeftEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::onEnter(const AimStopEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::onEnter(const DisarmEndStateEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::onEnter(const EndMoveRightEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::update(float deltaTime)
{
    if (cameraComponent_ and cameraComponent_->fsmContext)
    {
        cameraComponent_->fsmContext->yaw += context_.rotateStateData_.rotateSpeed_ * deltaTime;
        if (cameraRotationPolicy_ == CameraRotationPolicy::rotateOnlyCameraIfAvaiable)
            return;
    }

    if (not context_.rotateToTarget)
    {
        auto rotation =
            context_.rigidbody.GetRotation() *
            glm::angleAxis(glm::radians(context_.rotateStateData_.rotateSpeed_ * deltaTime), glm::vec3(0.f, 1.f, 0.f));
        // /*DISABLED*/  DEBUG_LOG("newRotation " + std::to_string(rotation));
        context_.rigidbody.SetRotation(rotation);
    }
    else
    {
        if (context_.rotateToTargetProgress < 1.f)
        {
            context_.rotateToTargetProgress += (rotateSpeed_ * deltaTime);
            context_.rotateToTargetProgress = glm::clamp(context_.rotateToTargetProgress, 0.f, 1.f);

            auto newRotation = glm::slerp(context_.startRotation, context_.targetRotation, context_.rotateToTargetProgress);

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

void RotateStateBase::update(const RotateTargetEvent &event)
{
    if (not context_.rotateToTarget or context_.targetRotation != event.target)
    {
        context_.startRotation          = context_.rigidbody.GetRotation();
        context_.targetRotation         = event.target;
        context_.rotateToTarget         = true;
        context_.rotateToTargetProgress = 0.f;
    }
}

void RotateStateBase::update(const AimStartEvent &)
{
    if (context_.rotateStateData_.rotateSpeed_ > 0.f)
    {
        update(RotateLeftEvent{});
    }
    else
    {
        update(RotateRightEvent{});
    }
}

void RotateStateBase::setRotateLeftAnim()
{
    if (not rotateAnimationClips_.left.empty() and not context_.animator.isAnimationPlaying(rotateAnimationClips_.left))
    {
        context_.animator.ChangeAnimation(rotateAnimationClips_.left, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_);
    }
}

void RotateStateBase::setRotateRightAnim()
{
    if (not rotateAnimationClips_.right.empty() and not context_.animator.isAnimationPlaying(rotateAnimationClips_.right))
    {
        context_.animator.ChangeAnimation(rotateAnimationClips_.right, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_);
    }
}

void RotateStateBase::setCurrentAnim()
{
    if (not context_.rotateToTarget)
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
    else
    {
        auto d = glm::dot(context_.targetRotation, context_.startRotation);
        if (d > 0)
            setRotateLeftAnim();
        else
            setRotateRightAnim();
    }
}

void RotateStateBase::setCurrentRotation()
{
    if (context_.rotateStateData_.rotateSpeed_ > 0.f)
    {
        update(RotateLeftEvent{});
    }
    else
    {
        update(RotateRightEvent{});
    }
}

void RotateStateBase::setCurrentAnimAndRotation()
{
    setCurrentAnim();
    setCurrentRotation();
}

void RotateStateBase::updateAnimationClipNames(const RotateAnimationClips &rotateAnimationClips)
{
    if (rotateAnimationClips_ != rotateAnimationClips)
    {
        rotateAnimationClips_ = rotateAnimationClips;
        setCurrentAnim();
    }
}

const RotateAnimationClips &RotateStateBase::getCurrentAnimationClips() const
{
    return rotateAnimationClips_;
}
}  // namespace Components
}  // namespace GameEngine
