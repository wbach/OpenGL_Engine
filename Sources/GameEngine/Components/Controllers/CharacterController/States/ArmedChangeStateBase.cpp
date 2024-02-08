#include "ArmedChangeStateBase.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "Logger/Log.h"
#include "Utils.h"

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
void ArmedChangeStateBase::update(const WeaponStateEvent&)
{
    DEBUG_LOG("update(const WeaponStateEvent&)");
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        disarmWeapon();
    }
    else
    {
        equipWeapon();
    }
}
void ArmedChangeStateBase::update(const DrawArrowEvent&)
{
    context_.drawArrowEventCalled_ = true;
}

void ArmedChangeStateBase::update(const AimStopEvent&)
{
    context_.drawArrowEventCalled_ = false;
}
void ArmedChangeStateBase::update(const SprintStateChangeEvent&)
{
    DEBUG_LOG(" update SprintStateChangeEvent context_.sprintEventCalled_ = " + Utils::BoolToString(context_.sprintEventCalled_));
    context_.sprintEventCalled_ = not context_.sprintEventCalled_;
}
void ArmedChangeStateBase::update(const SprintStartEvent&)
{
    DEBUG_LOG(" update SprintStartEvent context_.sprintEventCalled_ = " + Utils::BoolToString(context_.sprintEventCalled_));
    context_.sprintEventCalled_ = true;
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
    context_.weaponArmedChangeState = FsmContext::WeaponArmedChangeState::Equip;
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
    context_.weaponArmedChangeState = FsmContext::WeaponArmedChangeState::Disarm;
}

void ArmedChangeStateBase::onLeave(const EquipEndStateEvent&)
{
    DEBUG_LOG("onLeave context_.drawArrowEventCalled_ = " + Utils::BoolToString(context_.drawArrowEventCalled_));
    DEBUG_LOG("onLeave context_.sprintEventCalled_ = " + Utils::BoolToString(context_.sprintEventCalled_));

    if (context_.drawArrowEventCalled_)
    {
        context_.characterController.pushEventToQueue(DrawArrowEvent{});
    }
    else if (context_.sprintEventCalled_)
    {
        context_.characterController.pushEventToQueue(SprintStateChangeEvent{});
    }

    unsubscribeAll();
    context_.drawArrowEventCalled_ = false;
    context_.sprintEventCalled_    = false;
}

void ArmedChangeStateBase::onLeave(const DisarmEndStateEvent&)
{
    unsubscribeAll();
}

void ArmedChangeStateBase::triggerChange()
{
    const auto& animName = armed_ ? context_.animClipNames.equip : context_.animClipNames.disarm;

    subscribeForTransitionAnimationFrame_ = context_.animator.SubscribeForAnimationFrame(
        animName,
        [this, isArmed = armed_]()
        {
            unsubscribe(subscribeForTransitionAnimationFrame_);
            if (isArmed)
            {
                jointPoseUpdater_->setEquipJointAsCurrent();
            }
            else
            {
                jointPoseUpdater_->setDisarmJointAsCurrent();
            }
        },
        armed_ ? context_.armTimeStamps.arm : context_.armTimeStamps.disarm);

    subscribeForTransitionAnimationEnd_ = context_.animator.SubscribeForAnimationFrame(
        animName,
        [this]()
        {
            unsubscribe(subscribeForTransitionAnimationEnd_);
            if (armed_)
            {
                context_.characterController.pushEventToQueue(EquipEndStateEvent{});
            }
            else
            {
                context_.characterController.pushEventToQueue(DisarmEndStateEvent{});
            }
            context_.weaponArmedChangeState = FsmContext::WeaponArmedChangeState::None;
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
