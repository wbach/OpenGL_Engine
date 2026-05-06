#include "AttackStateBase.h"

#include <Logger/Log.h>

#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/PlayStateType.h"
#include "GameEngine/Components/Gameplay/Attack/MeleeAttackComponent.h"
#include "Types.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
AttackStateBase::AttackStateBase(FsmContext &context, const std::vector<AttackAnimation> &clipnames,
                                 const std::optional<std::string> jointGroupName)
    : BaseState{context}
    , attackStatesContext(context.attackStatesContext)
    , attackClipNames{clipnames}
    , jointGroupName{jointGroupName}
{
}
void AttackStateBase::onEnter(DisarmedRunState &)
{
    attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::run;
}
void AttackStateBase::onEnter(DisarmedWalkState &)
{
    attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::walk;
}
void AttackStateBase::onEnter(ArmedRunState &)
{
    attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::run;
}
void AttackStateBase::onEnter(ArmedWalkState &)
{
    attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::walk;
}
void AttackStateBase::onEnter(const AttackEvent &)
{
    startPlayCurrentAnimation();
}

void AttackStateBase::onEnter(const EndForwardMoveEvent &)
{
    attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndBackwardMoveEvent &)
{
    attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndMoveLeftEvent &)
{
    attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndMoveRightEvent &)
{
    attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const ChangeAnimEvent &event)
{
    update(event);
}

void AttackStateBase::update(const AttackEvent &)
{
    attackStatesContext.pendingAttacksCount = std::min(attackStatesContext.pendingAttacksCount + 1, 1u);
}

void AttackStateBase::update(const ChangeAnimEvent &event)
{
    context_.animator.ChangeAnimation(event.name, Animator::AnimationChangeType::smooth, PlayDirection::forward, jointGroupName);

    attackStatesContext.animationSubId = context_.animator.SubscribeForAnimationFrame(
        event.name, [&context = context_]() { context.characterController.pushEventToQueue(AnimationPlayEndEvent{}); });
}

void AttackStateBase::update(float)
{
}

void AttackStateBase::onLeave(const EndAttackEvent &)
{
    LOG_DEBUG << magic_enum::enum_name(attackStatesContext.nextMoveState);

    if (attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::run or
        attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::walk)
    {
        context_.characterController.pushEventToQueue(MoveEvent{});
    }

    if (attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::walk)
    {
        context_.characterController.pushEventToQueue(WalkChangeStateEvent{});
    }

    flushEvents();

    attackStatesContext.nextMoveState       = AttackStatesContext::NextMoveState::idle;
    attackStatesContext.pendingAttacksCount = 0;
    attackStatesContext.currentAnimation    = 0;
    attackStatesContext.currentSequenceClip = 0;
}
bool AttackStateBase::entryCondition() const
{
    return not attackClipNames.empty();
}
const std::optional<std::pair<std::string_view, PlayStateType>> AttackStateBase::getCurrentAnimation() const
{
    if (attackClipNames.empty())
    {
        LOG_DEBUG << "no clips";
        return {};
    }

    const auto &currentAnimation = attackClipNames[attackStatesContext.currentAnimation];
    const auto &clipsSequence    = currentAnimation.clipsSequence;
    if (clipsSequence.empty())
    {
        LOG_DEBUG << "clipsSequence empty";
        return {};
    }

    return std::pair<std::string_view, PlayStateType>{clipsSequence[attackStatesContext.currentSequenceClip],
                                                      currentAnimation.stateType};
}
void AttackStateBase::update(const AnimationPlayEndEvent &)
{
    if (attackStatesContext.animationSubId)
    {
        context_.animator.UnSubscribeForAnimationFrame(*attackStatesContext.animationSubId);
        attackStatesContext.animationSubId.reset();
    }

    auto meleAttackComponent = context_.gameObject.GetComponent<MeleeAttackComponent>();
    if (not meleAttackComponent)
    {
        LOG_DEBUG << "meleAttackComponent not found";
    }

    const auto &currentSequence = attackClipNames[attackStatesContext.currentAnimation].clipsSequence;
    if (attackStatesContext.currentSequenceClip < currentSequence.size() - 1)
    {
        ++attackStatesContext.currentSequenceClip;

        if (attackStatesContext.currentSequenceClip == attackStatesContext.hitAnimIdxInSeq)
        {
            if (meleAttackComponent)
                meleAttackComponent->StartAttack();
        }
        else
        {
            if (meleAttackComponent)
                meleAttackComponent->EndAttack();
        }
    }
    else
    {
        attackStatesContext.currentSequenceClip = 0;
        if (attackStatesContext.pendingAttacksCount > 0)
        {
            --attackStatesContext.pendingAttacksCount;
            attackStatesContext.currentAnimation = (attackStatesContext.currentAnimation + 1) % attackClipNames.size();
        }
        else
        {
            attackStatesContext.currentAnimation = 0;
            context_.characterController.pushEventToQueue(EndAttackEvent{});
            return;
        }
    }

    startPlayCurrentAnimation();
}
void AttackStateBase::startPlayCurrentAnimation()
{
    if (auto clip = getCurrentAnimation())
    {
        const auto &[clipName, stateType]    = *clip;
        attackStatesContext.currentPlayState = stateType;
        context_.characterController.pushEventToQueue(ChangeAnimEvent{std::string(clipName), stateType});
    }
    else
    {
        LOG_WARN << "Co active clip";
    }
}
}  // namespace Components
}  // namespace GameEngine
