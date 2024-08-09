#include "AttackStateBase.h"

#include <Logger/Log.h>

#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const AttackAnimation dummyAttackAnimation{};
}
AttackStateBase::AttackStateBase(FsmContext &context, const std::vector<AttackAnimation> &clipnames,
                                 const std::optional<std::string> jointGroupName)
    : context{context}
    , queue{context.attackStatesContext.queue}
    , attackClipNames{clipnames}
    , jointGroupName{jointGroupName}
{
}
void AttackStateBase::onEnter(DisarmedRunState &)
{
    context.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::run;
}
void AttackStateBase::onEnter(DisarmedWalkState &)
{
    context.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::walk;
}
void AttackStateBase::onEnter(ArmedRunState &)
{
    context.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::run;
}
void AttackStateBase::onEnter(ArmedWalkState &)
{
    context.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::walk;
}
void AttackStateBase::onEnter(const AttackEvent &)
{
    if (not attackClipNames.empty())
    {
        const auto &clipName = attackClipNames[context.attackStatesContext.currentAnimation].name;
        context.animator.ChangeAnimation(clipName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                         jointGroupName);
    }
}

void AttackStateBase::onEnter()
{
    if (not attackClipNames.empty())
    {
        subscribe();
    }
}

void AttackStateBase::onEnter(const EndForwardMoveEvent &)
{
    context.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context.animator.StopAnimation(context.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndBackwardMoveEvent &)
{
    context.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context.animator.StopAnimation(context.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndMoveLeftEvent &)
{
    context.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context.animator.StopAnimation(context.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndMoveRightEvent &)
{
    context.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context.animator.StopAnimation(context.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndRotationEvent &)
{
    DEBUG_LOG("EndRotationEvent");
    context.animator.StopAnimation(context.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const ChangeAnimEvent &event)
{
    update(event);
}

void AttackStateBase::update(const AttackEvent &)
{
    if (context.attackStatesContext.sequenceSize == context.attackStatesContext.currentAnimation)
    {
        if (context.attackStatesContext.sequenceSize < attackClipNames.size() - 1)
            ++context.attackStatesContext.sequenceSize;
        else
            context.attackStatesContext.sequenceSize = 0;
    }
}

void AttackStateBase::update(const ChangeAnimEvent &event)
{
    context.animator.ChangeAnimation(event.name, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                     jointGroupName);
}

void AttackStateBase::update(float)
{
}

void AttackStateBase::onLeave(const EndAttackEvent &)
{
    if (context.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::run or
        context.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::walk)
    {
        context.characterController.pushEventToQueue(MoveEvent{});
    }

    if (context.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::walk)
    {
        context.characterController.pushEventToQueue(WalkChangeStateEvent{});
    }

    for (const auto &e : queue)
    {
        context.characterController.pushEventToQueue(e);
    }
    queue.clear();

    context.attackStatesContext.nextMoveState    = AttackStatesContext::NextMoveState::idle;
    context.attackStatesContext.sequenceSize     = 0;
    context.attackStatesContext.currentAnimation = 0;
}

void AttackStateBase::onLeave()
{
    unsubscribe();
}
void AttackStateBase::onClipEnd()
{
    DEBUG_LOG("onClipEnd sequenceSize=" + std::to_string(context.attackStatesContext.sequenceSize));
    if (context.attackStatesContext.sequenceSize == context.attackStatesContext.currentAnimation)
    {
        context.characterController.pushEventToQueue(EndAttackEvent{});
        return;
    }

    context.attackStatesContext.currentAnimation = context.attackStatesContext.sequenceSize;
    const auto &clip                             = attackClipNames[context.attackStatesContext.currentAnimation];
    context.characterController.pushEventToQueue(ChangeAnimEvent{clip.name, clip.stateType});
}

void AttackStateBase::subscribe()
{
    for (const auto &clip : attackClipNames)
    {
        auto subId = context.animator.SubscribeForAnimationFrame(clip.name, [&]() { onClipEnd(); });
        subIds.push_back(subId);
    }
}

void AttackStateBase::unsubscribe()
{
    if (not subIds.empty())
    {
        for (const auto &id : subIds)
            context.animator.UnSubscribeForAnimationFrame(id);
        subIds.clear();
    }
}
const AttackAnimation &AttackStateBase::getCurrentAttackAnimation() const
{
    if (context.attackStatesContext.sequenceSize > 0 and
        context.attackStatesContext.sequenceSize < attackClipNames.size())
        return attackClipNames[context.attackStatesContext.sequenceSize];
    else
        return dummyAttackAnimation;
}
}  // namespace Components
}  // namespace GameEngine
