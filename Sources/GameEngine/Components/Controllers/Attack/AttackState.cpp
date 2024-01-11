#include "AttackState.h"

#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
AttackState::AttackState(AttackFsmContext &context)
    : context_{context}
    , animationIndex_{0}
    , currentAnimIndex_{0}
{
    subscribeForAnimationsEnd();
}
AttackState::AttackState(AttackState &&state)
    : context_(state.context_)
    , animationIndex_{state.animationIndex_}
    , currentAnimIndex_{state.currentAnimIndex_}
{
    subscribeForAnimationsEnd();
}
AttackState::~AttackState()
{
    for (auto &id : onAnimationEndSubIds)
    {
        context_.animator.UnSubscribeForAnimationFrame(id);
    }
}
void AttackState::onEnter(const AttackFsmEvents::Attack &)
{
    context_.jointGroupName.reset();

    if (not context_.attackAnimationNames.empty())
    {
        context_.animator.ChangeAnimation(context_.attackAnimationNames.front(), Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward);
    }
}
void AttackState::onEnter(const AttackFsmEvents::AttackGrouped &event)
{
    context_.jointGroupName = event.groupName;

    if (not context_.attackAnimationNames.empty())
    {
        context_.animator.ChangeAnimation(context_.attackAnimationNames.front(), Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, event.groupName);
    }
}
void AttackState::update(const AttackFsmEvents::Attack &)
{
    if (animationIndex_ == currentAnimIndex_)
    {
        ++animationIndex_;

        if (animationIndex_ >= context_.attackAnimationNames.size())
        {
            animationIndex_ = 0;
        }
    }
}
void AttackState::update(const AttackFsmEvents::AttackGrouped &)
{
    if (animationIndex_ == currentAnimIndex_)
    {
        ++animationIndex_;

        if (animationIndex_ >= context_.attackAnimationNames.size())
        {
            animationIndex_ = 0;
        }
    }
}
void AttackState::onLeave()
{
    animationIndex_   = 0;
    currentAnimIndex_ = 0;
}
void AttackState::subscribeForAnimationsEnd()
{
    if (context_.sendEndEventCallback)
    {
        for (const auto &animationName : context_.attackAnimationNames)
        {
            onAnimationEndSubIds.push_back(context_.animator.SubscribeForAnimationFrame(animationName, [&]() {
                if (animationIndex_ != currentAnimIndex_ and animationIndex_ < context_.attackAnimationNames.size() and
                    not context_.attackAnimationNames[animationIndex_].empty())
                {
                    currentAnimIndex_ = animationIndex_;

                    if (not context_.attackAnimationNames.empty())
                    {
                        context_.animator.ChangeAnimation(context_.attackAnimationNames[currentAnimIndex_],
                                                          Animator::AnimationChangeType::direct, PlayDirection::forward,
                                                          context_.jointGroupName);
                    }
                }
                else
                {
                    animationIndex_   = 0;
                    currentAnimIndex_ = 0;
                    context_.sendEndEventCallback();
                }
            }));
        }
    }
    else
    {
        WARNING_LOG("sendEndEventCallback not set");
    }
}
}  // namespace Components
}  // namespace GameEngine
