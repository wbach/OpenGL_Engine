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
        //, sut{context, animationClipInfo, 0.f, [&]() { isMainTransitionEnd = true; }}
    {
        machine.transitionTo<AnimationTransition>(context, animationClipInfo, 0.f, [&]() { isMainTransitionEnd = true; });
    }

    Pose currentPose;
    JointGroupsIds jointGroups;
    StateMachine machine;

    Context context;
    AnimationClipInfo animationClipInfo;
   // AnimationTransition sut;
    bool isMainTransitionEnd{false};

    AnimationClipInfo createClip(const std::string& name, float duration = CLIP_LENGTH)
    {
        AnimationClipInfo clipInfo;
        clipInfo.clip.name = name;
        clipInfo.clip.AddFrame(Animation::KeyFrame{});
        clipInfo.clip.AddFrame(Animation::KeyFrame{{duration}, {}});
        return clipInfo;
    }

    template <typename State>
    State* getStateIfActive()
    {
        return dynamic_cast<State*>(machine.currentState_.get());
    }
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
    auto newAnimationClipInfo = createClip("DummyAnimationClipName2");
    bool isTransitionEnd{false};
    machine.currentState_->handle(ChangeAnimationEvent{0.f, newAnimationClipInfo, std::nullopt, [&]() { isTransitionEnd = true; }});

    auto result = getStateIfActive<AnimationTransition>();
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
    machine.currentState_->handle(ChangeAnimationEvent{0.f, newAnimationClipInfo, "lower", [&]() { isTransitionEnd = true; }});

    auto result = getStateIfActive<AnimationTransitionToMixed>();
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


TEST_F(AnimationTransitionTests, ChangeAnimationEventWithNonExistingGroup)
{
    auto newAnimationClipInfo = createClip("DummyAnimationClipName2");
    bool isTransitionEnd{false};
    machine.currentState_->handle(ChangeAnimationEvent{0.f, newAnimationClipInfo, "notExistingJointGroup", [&]() { isTransitionEnd = true; }});

    auto result = getStateIfActive<AnimationTransition>();
    EXPECT_TRUE(result);
    const auto& resultAnims = result->getCurrentAnimation();

    auto it = std::find(resultAnims.begin(), resultAnims.end(), animationClipInfo.clip.name);
    EXPECT_TRUE(it != resultAnims.end());

    result->update(DEFAULT_ANIMATION_TRANSITION_TIME + EPSILON);
    EXPECT_FALSE(isTransitionEnd);
    EXPECT_TRUE(isMainTransitionEnd);
}
