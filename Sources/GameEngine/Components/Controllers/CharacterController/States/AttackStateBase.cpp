#include "AttackStateBase.h"

#include <Logger/Log.h>

#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine
{
namespace Components
{
AttackStateBase::AttackStateBase(FsmContext &context, const std::vector<AttackAnimation> &clipnames,
                                 const std::optional<std::string> jointGroupName)
    : context{context}
    , attackClipNames{clipnames}
    , jointGroupName{jointGroupName}
{
}
void AttackStateBase::onEnter(const AttackEvent &)
{
    DEBUG_LOG("DisarmedAttackState::onEnter(const AttackEvent &)");

    if (not attackClipNames.empty())
    {
        const auto &clipName = attackClipNames[currentAnimation].name;
        context.animator.ChangeAnimation(clipName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                         jointGroupName);

        subscribe();
    }
}

void AttackStateBase::onEnter(const EndForwardMoveEvent &)
{
    context.animator.StopAnimation(context.lowerBodyGroupName);
    subscribe();
}

void AttackStateBase::onEnter(const EndBackwardMoveEvent &)
{
    context.animator.StopAnimation(context.lowerBodyGroupName);
    subscribe();
}

void AttackStateBase::onEnter(const EndMoveLeftEvent &)
{
    context.animator.StopAnimation(context.lowerBodyGroupName);
    subscribe();
}

void AttackStateBase::onEnter(const EndMoveRightEvent &)
{
    context.animator.StopAnimation(context.lowerBodyGroupName);
    subscribe();
}

void AttackStateBase::update(const AttackEvent &)
{
    if (sequenceSize < attackClipNames.size() - 1 and sequenceSize == currentAnimation)
        ++sequenceSize;
}

void AttackStateBase::update(float)
{
}

template<typename ...States>
bool AttackStateBase::isAnyOfStateQueued()
{
    auto iter = std::find_if(queue.begin(), queue.end(), [](const auto& event)
    {
        return (std::holds_alternative<States>(event) or ...);
    });

    return iter!= queue.end();
}

void AttackStateBase::onLeave(const EndAttackEvent &)
{
    if (context.fsm->isPreviousStateOfType<DisarmedRunState>() or context.fsm->isPreviousStateOfType<ArmedRunState>())
    {
        if (not context.characterController.isAnyOfStateQueued<EndForwardMoveEvent, EndBackwardMoveEvent, EndMoveLeftEvent, EndMoveRightEvent>())
        {
            DEBUG_LOG("not isAnyOfStateQueued, queue.size=" + std::to_string(queue.size()));
            context.characterController.pushEventToQueue(MoveEvent{});
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
    if (sequenceSize == currentAnimation)
    {
        context.characterController.pushEventToQueue(EndAttackEvent{});
        return;
    }

    currentAnimation     = sequenceSize;
    const auto &clipName = attackClipNames[currentAnimation].name;
    context.animator.ChangeAnimation(clipName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                     jointGroupName);
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
}  // namespace Components
}  // namespace GameEngine
