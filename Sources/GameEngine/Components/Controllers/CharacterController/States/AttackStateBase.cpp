#include "AttackStateBase.h"

#include <Logger/Log.h>

#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine
{
namespace Components
{
AttackStateBase::AttackStateBase(FsmContext &context, const std::vector<AttackAnimation> &clipnames)
    : context{context}
    , attackClipNames{clipnames}
{
}
void AttackStateBase::onEnter(const AttackEvent &)
{
    DEBUG_LOG("DisarmedAttackState::onEnter(const AttackEvent &)");

    if (not attackClipNames.empty())
    {
        const auto &clipName = attackClipNames[currentAnimation].name;
        context.animator.ChangeAnimation(clipName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                         std::nullopt);

        for (const auto &clip : attackClipNames)
        {
            auto subId = context.animator.SubscribeForAnimationFrame(clip.name, [&]() { onClipEnd(); });
            subIds.push_back(subId);
        }
    }
}

void AttackStateBase::update(const AttackEvent &)
{
    if (sequenceSize < attackClipNames.size() - 1 and sequenceSize == currentAnimation)
        ++sequenceSize;
}

void AttackStateBase::update(const EndForwardMoveEvent &e)
{
    queue.push_back(e);
}

void AttackStateBase::update(const EndBackwardMoveEvent &e)
{
    queue.push_back(e);
}

void AttackStateBase::update(const EndMoveLeftEvent &e)
{
    queue.push_back(e);
}

void AttackStateBase::update(const EndMoveRightEvent &e)
{
    queue.push_back(e);
}
void AttackStateBase::update(float)
{
}
void AttackStateBase::onLeave()
{
    unsubscribe();
    sequenceSize     = 0;
    currentAnimation = 0;

    if (context.fsm->isPreviousStateOfType<DisarmedRunState>() or context.fsm->isPreviousStateOfType<ArmedRunState>())
    {
        context.characterController.pushEventToQueue(MoveEvent{});
        for(const auto& e : queue)
        {
            context.characterController.pushEventToQueue(e);
        }
        queue.clear();
    }
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
                                     std::nullopt);
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
