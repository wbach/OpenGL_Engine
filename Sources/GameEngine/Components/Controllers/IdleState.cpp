#include "IdleState.h"

#include "Attack/AttackEvents.h"

namespace GameEngine
{
namespace Components
{
IdleState::IdleState(FsmContext &context, const std::string &idleAnimName, const std::string &disarmAnimName)
    : context_{context}
    , idleAnimName_{idleAnimName}
    , disarmAnimName_{disarmAnimName}
{
}
void IdleState::onEnter()
{
    if (not idleAnimName_.empty() and not weaponChangeTriggered_)
    {
        context_.animator.ChangeAnimation(
            idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void IdleState::onEnter(const WeaponStateEvent &)
{
    if (not idleAnimName_.empty())
    {
        subscribeForTransitionAnimationEnd_ = context_.animator.SubscribeForAnimationEnd(
            disarmAnimName_,
            [this]()
            {
                context_.animator.ChangeAnimation(
                    idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                    context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);

                context_.animator.UnSubscribeForAnimationEnd(*subscribeForTransitionAnimationEnd_);
                weaponChangeTriggered_ = false;
            });

        context_.animator.ChangeAnimation(
            disarmAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);

        weaponChangeTriggered_ = true;
    }
}

void IdleState::update(const AttackEvent &)
{
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void IdleState::update(const EndAttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::End{});
    context_.multiAnimations = false;
    onEnter();
}
void IdleState::update(float)
{
}

void IdleState::leave()
{
    weaponChangeTriggered_ = false;
    context_.animator.UnSubscribeForAnimationEnd(*subscribeForTransitionAnimationEnd_);
}

}  // namespace Components
}  // namespace GameEngine
