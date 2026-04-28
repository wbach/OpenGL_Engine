#include "ArmedChangeStateBase.h"

#include <Logger/Log.h>
#include <Utils.h>

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/PoseUpdater.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ArmedChangeStateBase::ArmedChangeStateBase(FsmContext& context, const std::optional<std::string>& jointGroupName)
    : BaseState{context}
    , jointGroupName_{jointGroupName}
    , poseUpdater_{context.gameObject.GetComponentInChild<PoseUpdater>()}
{
}
void ArmedChangeStateBase::update(const WeaponStateEvent&)
{
    unsubscribeAll();
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
    context_.sprintEventCalled_ = not context_.sprintEventCalled_;
}
void ArmedChangeStateBase::equipWeapon()
{
    poseUpdater_ = context_.gameObject.GetComponentInChild<PoseUpdater>();
    if (not poseUpdater_)
    {
        LOG_DEBUG << "not poseUpdater_ return";
        return;
    }

    armed_ = true;
    triggerChange();
    context_.weaponArmedChangeState = FsmContext::WeaponArmedChangeState::Equip;
}

void ArmedChangeStateBase::disarmWeapon()
{
    if (not poseUpdater_)
    {
        return;
    }

    armed_ = false;
    triggerChange();
    context_.weaponArmedChangeState = FsmContext::WeaponArmedChangeState::Disarm;
}

void ArmedChangeStateBase::onLeave()
{
    flushEvents();
}

void ArmedChangeStateBase::onLeave(const EquipEndStateEvent&)
{
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
    const auto& animationSequence = armed_ ? context_.animClipNames.equip.clipNames : context_.animClipNames.disarm.clipNames;
    LOG_DEBUG << animationSequence.size();

    if (animationSequence.empty())
    {
        return;
    }

    if (animationSequence.size() > 2)
    {
        LOG_WARN << "Sequence len > 2 is not implemented";
        return;
    }

    const auto& firstClip = animationSequence.front();
    const auto& lastClip  = animationSequence.back();
    bool isSequence       = animationSequence.size() == 2;

    auto frameCallback = [this, isArmed = armed_, isSequence, next = lastClip]()
    {
        unsubscribe(subscribeForTransitionAnimationFrame_);

        if (isArmed)
        {
            poseUpdater_->setEquipJointAsCurrent();
        }
        else
        {
            poseUpdater_->setDisarmJointAsCurrent();
        }

        if (isSequence)
        {
            LOG_DEBUG << "First part finished. Change to next: " << next;
            context_.animator.ChangeAnimation(next, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                              jointGroupName_);
        }
    };

    auto endCallback = [this]()
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
    };

    const auto& timeStamp = isSequence ? -1.f : (armed_ ? context_.armTimeStamps.arm : context_.armTimeStamps.disarm);

    subscribeForTransitionAnimationFrame_ =
        context_.animator.SubscribeForAnimationFrame(firstClip, std::move(frameCallback), timeStamp);
    subscribeForTransitionAnimationEnd_ = context_.animator.SubscribeForAnimationFrame(lastClip, std::move(endCallback));

    context_.animator.ChangeAnimation(firstClip, Animator::AnimationChangeType::smooth, PlayDirection::forward, jointGroupName_);
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
bool ArmedChangeStateBase::hasWeapon() const
{
    return poseUpdater_ != nullptr or context_.gameObject.HasComponentInChild<PoseUpdater>();
}
}  // namespace Components
}  // namespace GameEngine
