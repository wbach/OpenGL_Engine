#include "AnimationStateFixture.h"

namespace AnimationStateTests
{
const float CLIP_LENGTH = 1.f;
const float EPSILON     = std::numeric_limits<float>::epsilon();
const std::string lowerJointGroupName{"lower"};
const std::string upperJointGroupName{"upper"};
const std::string nonExistingJointGroupName{"notExistingJointGroup"};
const std::string incominClipName{"incominClipName"};

AnimationStateFixture::AnimationStateFixture()
    : jointGroups{{lowerJointGroupName, {0}}, {upperJointGroupName, {1}}}
    , machine{currentPose, jointGroups}
    , context{currentPose, machine, jointGroups}
    , animationClipInfo{createClip("DummyAnimationClipName")}
{
}

AnimationClipInfo AnimationStateFixture::createClip(const std::string &name, float duration, AnimationClipInfo::PlayType playType)
{
    AnimationClipInfo clipInfo;
    clipInfo.playType = playType;

    clipInfo.clip = Animation::AnimationClip(name);
    clipInfo.clip.AddFrame(Animation::KeyFrame{});
    clipInfo.clip.AddFrame(Animation::KeyFrame{{duration}, {}});
    return clipInfo;
}

void AnimationStateFixture::expectClipName(const std::string &name)
{
    const auto &anims = machine.currentState_->getCurrentAnimation();
    auto it           = std::find(anims.begin(), anims.end(), name);
    EXPECT_TRUE(it != anims.end());
}

void AnimationStateFixture::expectClipName(const AnimationClipInfo &info)
{
    expectClipName(info.clip.getName());
}

void AnimationStateFixture::expectNoClipName(const std::string &name)
{
    const auto &anims = machine.currentState_->getCurrentAnimation();
    auto it           = std::find(anims.begin(), anims.end(), name);
    EXPECT_TRUE(it == anims.end());
}

void AnimationStateFixture::expectNoClipName(const AnimationClipInfo &info)
{
    expectClipName(info.clip.getName());
}
}  // namespace AnimationStateTests
