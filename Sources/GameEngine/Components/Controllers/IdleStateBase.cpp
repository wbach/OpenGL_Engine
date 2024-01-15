#include "IdleStateBase.h"

#include "Attack/AttackEvents.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
IdleStateBase::IdleStateBase(FsmContext &context, float armChangeTimeStamp, const std::string &idleAnimName,
                             const std::string &armChangeAnimName)
    : context_{context}
    , idleAnimName_{idleAnimName}
    , armChangeAnimName_{armChangeAnimName}
    , armChangeTimeStamp_{armChangeTimeStamp}
{
}
void IdleStateBase::onEnter()
{
    if (not idleAnimName_.empty() and not weaponChangeTriggered_)
    {
        context_.animator.ChangeAnimation(
            idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void IdleStateBase::onEnter(const WeaponStateEvent &)
{
    if (not idleAnimName_.empty())
    {
        subscribeForTransitionAnimationFrame_ = context_.animator.SubscribeForAnimationFrame(
            armChangeAnimName_,
            [this]()
            {
                setWeaponPosition();
                unsubscribe(subscribeForTransitionAnimationFrame_);
            },
            armChangeTimeStamp_);

        subscribeForTransitionAnimationEnd_ = context_.animator.SubscribeForAnimationFrame(
            armChangeAnimName_,
            [this]()
            {
                context_.animator.ChangeAnimation(
                    idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                    context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);

                unsubscribe(subscribeForTransitionAnimationEnd_);
                weaponChangeTriggered_ = false;
            });

        context_.animator.ChangeAnimation(
            armChangeAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);

        weaponChangeTriggered_ = true;
    }
}

void IdleStateBase::update(const AttackEvent &)
{
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void IdleStateBase::update(const EndAttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::End{});
    context_.multiAnimations = false;
    onEnter();
}

void IdleStateBase::update(float)
{
}

void IdleStateBase::onLeave()
{
    unsubscribeAll();
}

void IdleStateBase::unsubscribe(std::optional<uint32> &maybeId)
{
    if (maybeId)
    {
        context_.animator.UnSubscribeForAnimationFrame(*maybeId);
        maybeId = std::nullopt;
    }
}

void IdleStateBase::unsubscribeAll()
{
    weaponChangeTriggered_ = false;
    unsubscribe(subscribeForTransitionAnimationEnd_);
    unsubscribe(subscribeForTransitionAnimationFrame_);
}

}  // namespace Components
}  // namespace GameEngine
