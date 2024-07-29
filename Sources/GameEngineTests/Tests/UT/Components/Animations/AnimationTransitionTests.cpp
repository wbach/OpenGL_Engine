#include <gtest/gtest.h>

#include "GameEngine/Components/Animation/AnimationTransition.h"
#include "GameEngine/Components/Animation/AnimationTransitionToMixed.h"
#include "GameEngine/Components/Animation/EmptyState.h"
#include "GameEngine/Components/Animation/PlayAnimation.h"
#include "GameEngine/Components/Animation/StateMachine.h"

using namespace GameEngine;
using namespace GameEngine::Components;

const float CLIP_LENGTH = 1.f;
const float EPSILON     = std::numeric_limits<float>::epsilon();

struct AnimationTransitionTests : public ::testing::Test
{
    AnimationTransitionTests()
        : jointGroups{{"lower", {0}}, {"upper", {1}}}
        , machine{currentPose, jointGroups}
        , context{currentPose, machine, jointGroups}
        , animationClipInfo{createClip("DummyAnimationClipName")}
        , sut{context, animationClipInfo, 0.f, [&]() { isMainTransitionEnd = true; }}
    {
    }

    Pose currentPose;
    JointGroupsIds jointGroups;
    StateMachine machine;

    Context context;
    AnimationClipInfo animationClipInfo;
    AnimationTransition sut;
    bool isMainTransitionEnd{false};

    AnimationClipInfo createClip(const std::string& name, float duration = CLIP_LENGTH)
    {
        AnimationClipInfo clipInfo;
        clipInfo.clip.name = name;
        clipInfo.clip.AddFrame(Animation::KeyFrame{});
        clipInfo.clip.AddFrame(Animation::KeyFrame{{duration}, {}});
        return clipInfo;
    }
};

TEST_F(AnimationTransitionTests, transitionSuccess)
{
    sut.update(DEFAULT_ANIMATION_TRANSITION_TIME + EPSILON);
    auto result = dynamic_cast<PlayAnimation*>(machine.currentState_.get());
    EXPECT_TRUE(result);
    EXPECT_TRUE(isMainTransitionEnd);
}

TEST_F(AnimationTransitionTests, StopAnimationEvent)
{
    sut.handle(StopAnimationEvent{});
    auto result = dynamic_cast<EmptyState*>(machine.currentState_.get());
    EXPECT_TRUE(result);
    EXPECT_FALSE(isMainTransitionEnd);
}

TEST_F(AnimationTransitionTests, ChangeAnimationEventWithoutGroup)
{
    auto newAnimationClipInfo = createClip("DummyAnimationClipName2");
    bool isTransitionEnd{false};
    sut.handle(ChangeAnimationEvent{0.f, newAnimationClipInfo, std::nullopt, [&]() { isTransitionEnd = true; }});

    auto result = dynamic_cast<AnimationTransition*>(machine.currentState_.get());
    EXPECT_TRUE(result);
    EXPECT_EQ(newAnimationClipInfo.clip.name, result->getCurrentAnimation().front());
    result->update(DEFAULT_ANIMATION_TRANSITION_TIME + EPSILON);
    EXPECT_TRUE(isTransitionEnd);
    EXPECT_FALSE(isMainTransitionEnd);
}

TEST_F(AnimationTransitionTests, ChangeAnimationEventWithGroup)
{
    auto newAnimationClipInfo = createClip("DummyAnimationClipName2");
    bool isTransitionEnd{false};
    sut.handle(ChangeAnimationEvent{0.f, newAnimationClipInfo, "lower", [&]() { isTransitionEnd = true; }});

    auto result = dynamic_cast<AnimationTransitionToMixed*>(machine.currentState_.get());
    EXPECT_TRUE(result);
    const auto& resultAnims = result->getCurrentAnimation();
    auto it                 = std::find(resultAnims.begin(), resultAnims.end(), newAnimationClipInfo.clip.name);
    EXPECT_TRUE(it != resultAnims.end());
    it = std::find(resultAnims.begin(), resultAnims.end(), animationClipInfo.clip.name);
    EXPECT_TRUE(it != resultAnims.end());

    result->update(DEFAULT_ANIMATION_TRANSITION_TIME + EPSILON);
    EXPECT_TRUE(isTransitionEnd);
    EXPECT_TRUE(isMainTransitionEnd);
}
