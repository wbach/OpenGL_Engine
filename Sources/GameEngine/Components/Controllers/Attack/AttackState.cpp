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
AttackState::AttackState(const AttackState &state)
    : context_(state.context_)
    , animationIndex_{state.animationIndex_}
    , currentAnimIndex_{state.currentAnimIndex_}
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
    DEBUG_LOG("~AttackState");
    for (auto &id : onAnimationEndSubIds)
    {
        context_.animator.UnSubscribeForAnimationEnd(id);
    }
}
void AttackState::onEnter(const AttackFsmEvents::Attack &)
{
    context_.jointGroupName.reset();
    DEBUG_LOG("onEnter(const AttackFsmEvents::Attack&) ");

    if (not context_.attackAnimationNames.empty())
    {
        context_.animator.ChangeAnimation(context_.attackAnimationNames.front(), Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward);
    }
}
void AttackState::onEnter(const AttackFsmEvents::AttackGrouped &event)
{
    context_.jointGroupName = event.groupName;

    DEBUG_LOG("onEnter(const AttackFsmEvents::AttackGrouped&) ");

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
            onAnimationEndSubIds.push_back(context_.animator.SubscribeForAnimationEnd(animationName, [&]() {
                if (animationIndex_ != currentAnimIndex_ and animationIndex_ < context_.attackAnimationNames.size() and
                    not context_.attackAnimationNames[animationIndex_].empty())
                {
                    currentAnimIndex_ = animationIndex_;

                    if (not context_.attackAnimationNames.empty())
                    {
                        DEBUG_LOG("Change anim : " + context_.attackAnimationNames[currentAnimIndex_]);
                        context_.animator.ChangeAnimation(context_.attackAnimationNames[currentAnimIndex_],
                                                          Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                                          context_.jointGroupName);
                    }
                }
                else
                {
                    animationIndex_   = 0;
                    currentAnimIndex_ = 0;
                    DEBUG_LOG("send end");
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
