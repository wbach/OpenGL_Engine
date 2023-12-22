#include "IdleStateWithWeapon.h"

#include "Attack/AttackEvents.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"

namespace GameEngine
{
namespace Components
{
IdleStateWithWeapon::IdleStateWithWeapon(FsmContext &context, const std::string &idleAnimName,
                                         const std::string &equipAnimName)
    : context_{context}
    , idleAnimName_{idleAnimName}
    , equipAnimName_{equipAnimName}
    , jointPoseUpdater_{context.gameObject.GetComponentInChild<JointPoseUpdater>()}
{
}
void IdleStateWithWeapon::onEnter()
{
}

void IdleStateWithWeapon::onEnter(const WeaponStateEvent &)
{
    if (not idleAnimName_.empty() and not equipAnimName_.empty())
    {
        subscribeForTransitionAnimationEnd_ = context_.animator.SubscribeForAnimationEnd(
            equipAnimName_,
            [this]()
            {
                context_.animator.ChangeAnimation(
                    idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                    context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);

                unsubscribe();

                if (jointPoseUpdater_)
                {
                    jointPoseUpdater_->setEquipJointAsCurrent();
                }
                else
                {
                    WARNING_LOG("jointPoseUpdater_ not set");
                }
            });

        context_.animator.ChangeAnimation(
            equipAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void IdleStateWithWeapon::update(const AttackEvent &)
{
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void IdleStateWithWeapon::update(const EndAttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::End{});
    context_.multiAnimations = false;
    onEnter();
}
void IdleStateWithWeapon::update(float)
{
}
void IdleStateWithWeapon::onLeave()
{
    unsubscribe();
}

void IdleStateWithWeapon::unsubscribe()
{
    if (subscribeForTransitionAnimationEnd_)
    {
        context_.animator.UnSubscribeForAnimationEnd(*subscribeForTransitionAnimationEnd_);
        subscribeForTransitionAnimationEnd_ = std::nullopt;
    }
}

}  // namespace Components
}  // namespace GameEngine
