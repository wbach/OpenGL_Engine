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
    : BaseState{context}
    , attackClipNames{clipnames}
    , jointGroupName{jointGroupName}
{
}
void AttackStateBase::onEnter(DisarmedRunState &)
{
    context_.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::run;
}
void AttackStateBase::onEnter(DisarmedWalkState &)
{
    context_.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::walk;
}
void AttackStateBase::onEnter(ArmedRunState &)
{
    context_.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::run;
}
void AttackStateBase::onEnter(ArmedWalkState &)
{
    context_.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::walk;
}
void AttackStateBase::onEnter(const AttackEvent &)
{
    if (not attackClipNames.empty())
    {
        const auto &clipName = attackClipNames[context_.attackStatesContext.currentAnimation].name;
        context_.animator.ChangeAnimation(clipName, Animator::AnimationChangeType::smooth, PlayDirection::forward, jointGroupName);
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
    context_.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndBackwardMoveEvent &)
{
    context_.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndMoveLeftEvent &)
{
    context_.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndMoveRightEvent &)
{
    context_.attackStatesContext.nextMoveState = AttackStatesContext::NextMoveState::idle;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndRotationEvent &)
{
    DEBUG_LOG("EndRotationEvent");
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const ChangeAnimEvent &event)
{
    update(event);
}

void AttackStateBase::update(const AttackEvent &)
{
    if (context_.attackStatesContext.sequenceSize == context_.attackStatesContext.currentAnimation)
    {
        if (context_.attackStatesContext.sequenceSize < attackClipNames.size() - 1)
            ++context_.attackStatesContext.sequenceSize;
        else
            context_.attackStatesContext.sequenceSize = 0;
    }
}

void AttackStateBase::update(const ChangeAnimEvent &event)
{
    context_.animator.ChangeAnimation(event.name, Animator::AnimationChangeType::smooth, PlayDirection::forward, jointGroupName);
}

void AttackStateBase::update(float)
{
}

void AttackStateBase::onLeave(const EndAttackEvent &)
{
    if (context_.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::run or
        context_.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::walk)
    {
        context_.characterController.pushEventToQueue(MoveEvent{});
    }

    if (context_.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::walk)
    {
        context_.characterController.pushEventToQueue(WalkChangeStateEvent{});
    }

    flushEvents();

    context_.attackStatesContext.nextMoveState    = AttackStatesContext::NextMoveState::idle;
    context_.attackStatesContext.sequenceSize     = 0;
    context_.attackStatesContext.currentAnimation = 0;
}

void AttackStateBase::onLeave()
{
    unsubscribe();
}
void AttackStateBase::onClipEnd()
{
    DEBUG_LOG("onClipEnd sequenceSize=" + std::to_string(context_.attackStatesContext.sequenceSize));
    if (context_.attackStatesContext.sequenceSize == context_.attackStatesContext.currentAnimation)
    {
        context_.characterController.pushEventToQueue(EndAttackEvent{});
        return;
    }

    context_.attackStatesContext.currentAnimation = context_.attackStatesContext.sequenceSize;
    const auto &clip                             = attackClipNames[context_.attackStatesContext.currentAnimation];
    context_.characterController.pushEventToQueue(ChangeAnimEvent{clip.name, clip.stateType});
}

void AttackStateBase::subscribe()
{
    for (const auto &clip : attackClipNames)
    {
        auto subId = context_.animator.SubscribeForAnimationFrame(clip.name, [&]() { onClipEnd(); });
        subIds.push_back(subId);
    }
}

void AttackStateBase::unsubscribe()
{
    if (not subIds.empty())
    {
        for (const auto &id : subIds)
            context_.animator.UnSubscribeForAnimationFrame(id);
        subIds.clear();
    }
}
const AttackAnimation &AttackStateBase::getCurrentAttackAnimation() const
{
    if (context_.attackStatesContext.sequenceSize > 0 and context_.attackStatesContext.sequenceSize < attackClipNames.size())
        return attackClipNames[context_.attackStatesContext.sequenceSize];
    else
        return dummyAttackAnimation;
}
}  // namespace Components
}  // namespace GameEngine
