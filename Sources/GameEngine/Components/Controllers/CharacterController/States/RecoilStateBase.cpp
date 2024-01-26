#include "RecoilStateBase.h"

#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilStateBase::RecoilStateBase(FsmContext& contex)
    : context_{contex}
    , animName_{context_.animClipNames.recoilArrow}
{
    context_.animator.setPlayOnceForAnimationClip(animName_);
}

void RecoilStateBase::onEnter(const EndRotationEvent&)
{
    context_.multiAnimations = false;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void RecoilStateBase::onEnter(const AttackEvent&)
{
    DEBUG_LOG("On enter DrawArrowEvent clip: " + animName_);

    if (animName_.empty())
    {
        return;
    }

    setAnim();

    context_.animator.SubscribeForAnimationFrame(
        animName_, [&]() { context_.characterController.fsm()->handle(DrawArrowEvent{}); });
}
void RecoilStateBase::update(float)
{
}
void RecoilStateBase::setAnim()
{
    context_.animator.ChangeAnimation(
        animName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
        context_.multiAnimations ? std::make_optional(context_.upperBodyGroupName) : std::nullopt);
}
}  // namespace Components
}  // namespace GameEngine
