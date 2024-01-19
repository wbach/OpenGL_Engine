
#include "StateBase.h"

#include "CharacterController.h"
#include "FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
StateBase::StateBase(FsmContext& context)
    : context_{context}
    , jointPoseUpdater_{context.gameObject.GetComponentInChild<JointPoseUpdater>()}
{
}

void StateBase::equipWeapon()
{
    if (not jointPoseUpdater_)
    {
        DEBUG_LOG("not jointPoseUpdater_");
        return;
    }

    armed_ = true;
    triggerChange();
}

void StateBase::disarmWeapon()
{
    if (not jointPoseUpdater_)
    {
        DEBUG_LOG("not jointPoseUpdater_");
        return;
    }

    armed_ = false;
    triggerChange();
}

void StateBase::triggerChange()
{
    context_.weaponChangeTriggered_ = true;

    const auto& animName = armed_ ? context_.animClipNames.equip : context_.animClipNames.disarm;

    DEBUG_LOG("triggerChange animName : " + animName);
    subscribeForTransitionAnimationFrame_ = context_.animator.SubscribeForAnimationFrame(
        animName,
        [this, isArmed = armed_]()
        {
            if (isArmed)
            {
                jointPoseUpdater_->setEquipJointAsCurrent();
            }
            else
            {
                jointPoseUpdater_->setDisarmJointAsCurrent();
            }

            unsubscribe(subscribeForTransitionAnimationFrame_);
        },
        armed_ ? context_.armTimeStamps.disarm : context_.armTimeStamps.disarm);

    subscribeForTransitionAnimationEnd_ = context_.animator.SubscribeForAnimationFrame(
        animName,
        [this]()
        {
            context_.characterController.fsm()->handle(WeaponChangeEndEvent{});
            unsubscribe(subscribeForTransitionAnimationEnd_);
            context_.weaponChangeTriggered_ = false;
        });

    context_.animator.ChangeAnimation(
        animName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
        context_.multiAnimations ? std::make_optional(context_.upperBodyGroupName) : std::nullopt);

    context_.weaponChangeTriggered_ = true;
}
void StateBase::unsubscribe(std::optional<uint32>& maybeId)
{
    if (maybeId)
    {
        context_.animator.UnSubscribeForAnimationFrame(*maybeId);
        maybeId = std::nullopt;
    }
}

void StateBase::unsubscribeAll()
{
    context_.weaponChangeTriggered_ = false;
    unsubscribe(subscribeForTransitionAnimationEnd_);
    unsubscribe(subscribeForTransitionAnimationFrame_);
}
}  // namespace Components
}  // namespace GameEngine
