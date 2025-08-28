#include <gtest/gtest.h>

#include "AnimationStateFixture.h"
#include "GameEngine/Components/Animation/AnimationTransition.h"
#include "GameEngine/Components/Animation/AnimationTransitionToMixed.h"
#include "GameEngine/Components/Animation/EmptyState.h"
#include "GameEngine/Components/Animation/PlayAnimation.h"

namespace AnimationStateTests
{

struct AnimationTransitionTests : public AnimationStateFixture
{
    AnimationTransitionTests()
    {
        machine.transitionTo<AnimationTransition>(context, animationClipInfo, 0.f,
                                                  [&]() { isMainTransitionEnd = true; });
    }

    bool isMainTransitionEnd{false};
};

TEST_F(AnimationTransitionTests, transitionSuccess)
{
    machine.currentState_->update(DEFAULT_ANIMATION_TRANSITION_TIME + EPSILON);
    auto result = getStateIfActive<PlayAnimation>();
    EXPECT_TRUE(result);
    EXPECT_TRUE(isMainTransitionEnd);
}

TEST_F(AnimationTransitionTests, StopAnimationEvent)
{
    machine.currentState_->handle(StopAnimationEvent{});
    auto result = getStateIfActive<EmptyState>();
    EXPECT_TRUE(result);
    EXPECT_FALSE(isMainTransitionEnd);
}

TEST_F(AnimationTransitionTests, ChangeAnimationEventWithoutGroup)
{
    auto incomingAnimationClipInfo = createClip(incominClipName);
    bool isTransitionEnd{false};
    machine.currentState_->handle(
        ChangeAnimationEvent{0.f, incomingAnimationClipInfo, std::nullopt, [&]() { isTransitionEnd = true; }});

    auto result = getStateIfActive<AnimationTransition>();
    EXPECT_TRUE(result);

    expectClipName(incomingAnimationClipInfo);

    result->update(DEFAULT_ANIMATION_TRANSITION_TIME + EPSILON);
    EXPECT_TRUE(isTransitionEnd);
    EXPECT_FALSE(isMainTransitionEnd);
}

TEST_F(AnimationTransitionTests, ChangeAnimationEventWithGroup)
{
    auto incomingAnimationClipInfo = createClip(incominClipName);
    bool isTransitionEnd{false};
    machine.currentState_->handle(
        ChangeAnimationEvent{0.f, incomingAnimationClipInfo, lowerJointGroupName, [&]() { isTransitionEnd = true; }});

    auto result = getStateIfActive<AnimationTransitionToMixed>();
    EXPECT_TRUE(result);

    expectClipName(incomingAnimationClipInfo);
    expectClipName(animationClipInfo);

    result->update(DEFAULT_ANIMATION_TRANSITION_TIME + EPSILON);
    EXPECT_TRUE(isTransitionEnd);
    EXPECT_TRUE(isMainTransitionEnd);
}

TEST_F(AnimationTransitionTests, ChangeAnimationEventWithNonExistingGroup)
{
    auto incomingAnimationClipInfo = createClip(incominClipName);
    bool isTransitionEnd{false};
    machine.currentState_->handle(
        ChangeAnimationEvent{0.f, incomingAnimationClipInfo, nonExistingJointGroupName, [&]() { isTransitionEnd = true; }});

    auto result = getStateIfActive<AnimationTransition>();
    EXPECT_TRUE(result);

    expectClipName(animationClipInfo);

    result->update(DEFAULT_ANIMATION_TRANSITION_TIME + EPSILON);
    EXPECT_FALSE(isTransitionEnd);
    EXPECT_TRUE(isMainTransitionEnd);
}
}  // namespace AnimationStateTests
