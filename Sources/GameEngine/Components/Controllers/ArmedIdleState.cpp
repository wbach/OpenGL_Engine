#include "ArmedIdleState.h"

#include "Attack/AttackEvents.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"

namespace GameEngine
{
namespace Components
{
ArmedIdleState::ArmedIdleState(FsmContext &context, float equipTimeStamp)
    : context_{context}
    , idleAnimName_{context.animClipNames.armed.idle}
    , equipAnimName_{context.animClipNames.equip}
    , equipTimeStamp_{equipTimeStamp}
    , jointPoseUpdater_{context.gameObject.GetComponentInChild<JointPoseUpdater>()}
{
}
void ArmedIdleState::onEnter()
{
}

void ArmedIdleState::onEnter(const WeaponStateEvent &)
{
    if (not idleAnimName_.empty() and not equipAnimName_.empty())
    {
        subscribeForTransitionAnimationEnd_ = context_.animator.SubscribeForAnimationFrame(
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
            },
            equipTimeStamp_);

        context_.animator.ChangeAnimation(
            equipAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void ArmedIdleState::update(const AttackEvent &)
{
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::Attack{});
}

void ArmedIdleState::update(const EndAttackEvent &)
{
    context_.attackFsm.handle(AttackFsmEvents::End{});
    context_.multiAnimations = false;
    onEnter();
}
void ArmedIdleState::update(float)
{
}
void ArmedIdleState::onLeave()
{
    unsubscribe();
}

void ArmedIdleState::unsubscribe()
{
    if (subscribeForTransitionAnimationEnd_)
    {
        context_.animator.UnSubscribeForAnimationFrame(*subscribeForTransitionAnimationEnd_);
        subscribeForTransitionAnimationEnd_ = std::nullopt;
    }
}

}  // namespace Components
}  // namespace GameEngine
