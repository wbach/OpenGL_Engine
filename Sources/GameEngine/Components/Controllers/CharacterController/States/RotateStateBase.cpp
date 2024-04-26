#include "RotateStateBase.h"

#include <Logger/Log.h>

#include "../FsmContext.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
namespace Components
{
RotateStateBase::RotateStateBase(FsmContext &context, const std::optional<std::string> &jointGourpName,
                                 float rotateSpeed, const std::string &rotateLeftAnim,
                                 const std::string &rotateRightAnim)
    : context_{context}
    , jointGroupName_{jointGourpName}
    , rotateLeftAnim_{rotateLeftAnim}
    , rotateRightAnim_{rotateRightAnim}
    , rotateSpeed_{rotateSpeed}
{
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

        setRotateLeftAnim();
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

void RotateStateBase::onEnter(const AimStopEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::onEnter(const DisarmEndStateEvent &)
{
    setCurrentAnim();
}

void RotateStateBase::update(const AttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void RotateStateBase::update(const EndAttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::End{});
}

void RotateStateBase::update(float deltaTime)
{
    if (not context_.rotateToTarget)
    {
        auto rotation =
            context_.rigidbody.GetRotation() *
            glm::angleAxis(glm::radians(context_.rotateStateData_.rotateSpeed_ * deltaTime), glm::vec3(0.f, 1.f, 0.f));
                    DEBUG_LOG("newRotation " + std::to_string(rotation));
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
    if (not rotateLeftAnim_.empty() and not context_.animator.isAnimationPlaying(rotateLeftAnim_))
    {
        context_.animator.ChangeAnimation(rotateLeftAnim_, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_);
    }
}

void RotateStateBase::setRotateRightAnim()
{
    if (not rotateRightAnim_.empty() and not context_.animator.isAnimationPlaying(rotateRightAnim_))
    {
        context_.animator.ChangeAnimation(rotateRightAnim_, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_);
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

void RotateStateBase::updateAnimationClipNames(const std::string &rotateLeftAnim, const std::string &rotateRightAnim)
{
    if (rotateLeftAnim_ != rotateLeftAnim or rotateRightAnim_ != rotateRightAnim)
    {
        rotateLeftAnim_ = rotateLeftAnim;
        rotateRightAnim_ = rotateRightAnim;
        setCurrentAnim();
    }
}

}  // namespace Components
}  // namespace GameEngine
