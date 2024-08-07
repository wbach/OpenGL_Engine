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
    , attackClipNames{clipnames}
    , jointGroupName{jointGroupName}
{
}
void AttackStateBase::onEnter(const AttackEvent &)
{
    if (not attackClipNames.empty())
    {
        const auto &clipName = attackClipNames[currentAnimation].name;
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
    context.animator.StopAnimation(context.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndBackwardMoveEvent &)
{
    context.animator.StopAnimation(context.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndMoveLeftEvent &)
{
    context.animator.StopAnimation(context.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const EndMoveRightEvent &)
{
    context.animator.StopAnimation(context.lowerBodyGroupName);
}

void AttackStateBase::onEnter(const ChangeAnimEvent &event)
{
    update(event);
}

void AttackStateBase::update(const AttackEvent &)
{
    if (sequenceSize == currentAnimation)
    {
        if (sequenceSize < attackClipNames.size() - 1)
            ++sequenceSize;
        else
            sequenceSize = 0;
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

template <typename... States>
bool AttackStateBase::isAnyOfStateQueued()
{
    auto iter = std::find_if(queue.begin(), queue.end(),
                             [](const auto &event) { return (std::holds_alternative<States>(event) or ...); });

    return iter != queue.end();
}

void AttackStateBase::onLeave(const EndAttackEvent &)
{
    auto wasRunState  = context.fsm->isPreviousStateOfType<DisarmedRunState, DisarmedAttackAndRunState, ArmedRunState,
                                                          ArmedAttackAndRunState>();
    auto wasWalkState = context.fsm->isPreviousStateOfType<DisarmedWalkState, DisarmedAttackAndWalkState,
                                                           ArmedWalkState, ArmedAttackAndWalkState>();

    if (wasRunState or wasWalkState)
    {
        if (not context.characterController.isAnyOfStateQueued<EndForwardMoveEvent, EndBackwardMoveEvent,
                                                               EndMoveLeftEvent, EndMoveRightEvent>())
        {
            if (wasWalkState)
            {
                context.characterController.pushEventToFrontQueue(WalkChangeStateEvent{});
            }
            context.characterController.pushEventToFrontQueue(MoveEvent{});
        }
    }
}

void AttackStateBase::onLeave()
{
    unsubscribe();
    sequenceSize     = 0;
    currentAnimation = 0;

    for (const auto &e : queue)
    {
        context.characterController.pushEventToQueue(e);
    }
    queue.clear();
}
void AttackStateBase::onClipEnd()
{
    DEBUG_LOG("onClipEnd sequenceSize=" + std::to_string(sequenceSize));
    if (sequenceSize == currentAnimation)
    {
        context.characterController.pushEventToQueue(EndAttackEvent{});
        return;
    }

    currentAnimation = sequenceSize;
    const auto &clip = attackClipNames[currentAnimation];
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
    if (sequenceSize > 0 and sequenceSize < attackClipNames.size())
        return attackClipNames[sequenceSize];
    else
        return dummyAttackAnimation;
}
}  // namespace Components
}  // namespace GameEngine
