#include <gtest/gtest.h>

#include "AnimationStateFixture.h"
#include "GameEngine/Components/Animation/AnimationTransition.h"
#include "GameEngine/Components/Animation/AnimationTransitionMixedToSingle.h"
#include "GameEngine/Components/Animation/AnimationTransitionToMixed.h"
#include "GameEngine/Components/Animation/EmptyState.h"
#include "GameEngine/Components/Animation/PlayMixedAnimation.h"

namespace AnimationStateTests
{
struct PlayMixedAnimationTests : public AnimationStateFixture
{
    PlayMixedAnimationTests()
        : incomingClipInfo{createClip(incominClipName)}
    {
        float startTime1{0.1f};
        float startIncommingTime1{0.3f};
        AnimationClipInfoPerGroup animationClipInfoPerGroup{
            {lowerJointGroupName, {animationClipInfo, startTime1}},
            {upperJointGroupName, {incomingClipInfo, startIncommingTime1}},
        };
        machine.transitionTo<PlayMixedAnimation>(context, animationClipInfoPerGroup);
    }
    AnimationClipInfo incomingClipInfo;
};

TEST_F(PlayMixedAnimationTests, stopSingleAnimation)
{
    machine.currentState_->handle(StopAnimationEvent{lowerJointGroupName});
    auto transitionToSingle = getStateIfActive<AnimationTransitionMixedToSingle>();
    EXPECT_TRUE(transitionToSingle);
}
}  // namespace AnimationStateTests
