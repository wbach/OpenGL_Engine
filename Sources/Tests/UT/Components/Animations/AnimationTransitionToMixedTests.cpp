#include <gtest/gtest.h>

#include "AnimationStateFixture.h"
#include "GameEngine/Components/Animation/AnimationTransition.h"
#include "GameEngine/Components/Animation/AnimationTransitionToMixed.h"
#include "GameEngine/Components/Animation/EmptyState.h"
#include "GameEngine/Components/Animation/PlayMixedAnimation.h"
#include "GameEngine/Components/Animation/AnimationTransitionMixedToSingle.h"
#include "GameEngine/Components/Animation/TransitionGroupsPlaying.h"
#include "GameEngine/Components/Animation/CurrentGroupsPlayingInfo.h"

namespace AnimationStateTests
{
struct AnimationTransitionToMixedTests : public AnimationStateFixture
{
    AnimationTransitionToMixedTests()
        : incomingClipInfo{createClip(incominClipName)}
        , changeAnimationEvent{0.f, incomingClipInfo, upperJointGroupName,  [&]() { isMainTransitionEnd = true; }}
        , currentGroupsPlayingInfo{{animationClipInfo, CLIP_LENGTH / 2.f, {lowerJointGroupName}}}
    {
        machine.transitionTo<AnimationTransitionToMixed>(context, currentGroupsPlayingInfo, transitionGroupsPlaying,
                                                         changeAnimationEvent);
    }
    AnimationClipInfo incomingClipInfo;
    ChangeAnimationEvent changeAnimationEvent;
    std::vector<CurrentGroupsPlayingInfo> currentGroupsPlayingInfo;
    std::vector<TransitionGroupsPlaying> transitionGroupsPlaying;
    bool isMainTransitionEnd{false};
};

TEST_F(AnimationTransitionToMixedTests, transitionToMixedSuccessAndBackToSingle)
{
    machine.currentState_->update(DEFAULT_ANIMATION_TRANSITION_TIME + EPSILON);
    auto playMixedAnim = getStateIfActive<PlayMixedAnimation>();
    EXPECT_TRUE(playMixedAnim);
    EXPECT_TRUE(isMainTransitionEnd);

    expectClipName(incomingClipInfo);
    expectClipName(animationClipInfo);

    machine.currentState_->update(CLIP_LENGTH / 2.f + EPSILON);
    auto transitionToSingle = getStateIfActive<AnimationTransitionMixedToSingle>();
    EXPECT_TRUE(transitionToSingle);
    expectClipName(incomingClipInfo);
}

}  // namespace AnimationStateTests
