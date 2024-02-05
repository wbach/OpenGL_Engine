#include "ArmedChangeStateBase.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
ArmedChangeStateBase::ArmedChangeStateBase(FsmContext& context, const std::optional<std::string>& jointGroupName)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , jointPoseUpdater_{context.gameObject.GetComponentInChild<JointPoseUpdater>()}
{
}

void ArmedChangeStateBase::equipWeapon()
{
    if (not jointPoseUpdater_)
    {
        DEBUG_LOG("not jointPoseUpdater_");
        return;
    }

    armed_ = true;
    triggerChange();
}

void ArmedChangeStateBase::disarmWeapon()
{
    if (not jointPoseUpdater_)
    {
        DEBUG_LOG("not jointPoseUpdater_");
        return;
    }

    armed_ = false;
    triggerChange();
}

void ArmedChangeStateBase::triggerChange()
{
    const auto& animName = armed_ ? context_.animClipNames.equip : context_.animClipNames.disarm;

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
        });

    context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                      jointGroupName_);
}
void ArmedChangeStateBase::unsubscribe(std::optional<uint32>& maybeId)
{
    if (maybeId)
    {
        context_.animator.UnSubscribeForAnimationFrame(*maybeId);
        maybeId = std::nullopt;
    }
}

void ArmedChangeStateBase::unsubscribeAll()
{
    unsubscribe(subscribeForTransitionAnimationEnd_);
    unsubscribe(subscribeForTransitionAnimationFrame_);
}
}  // namespace Components
}  // namespace GameEngine
