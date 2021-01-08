#include <gtest/gtest.h>

#include "BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"

using namespace GameEngine;
using namespace GameEngine::Animation;
using namespace GameEngine::Components;

namespace
{
const std::string CLIP_NAME{"DefaultAnimationClip"};
}

struct AnimatorTestWrapper : public Animator
{
    AnimatorTestWrapper(ComponentContext& context, GameObject& go)
        : Animator(context, go)
    {
    }
    std::pair<KeyFrame, KeyFrame> _getPreviousAndNextFrames()
    {
        return getPreviousAndNextFrames();
    }
    void SetTime(float t)
    {
        currentTime_ = t;
    }
};

struct AnimatorTestSchould : public BaseComponentTestSchould
{
    AnimatorTestSchould()
        : BaseComponentTestSchould()
        , sut_(context_, obj_)
    {
    }
    virtual void SetUp() override
    {
        auto& anim = sut_.animationClips_[CLIP_NAME];
        anim.name  = CLIP_NAME;

        KeyFrame frame;
        frame.timeStamp = 0;
        anim.AddFrame(frame);
        frame.timeStamp = 0.33f;
        anim.AddFrame(frame);
        frame.timeStamp = 0.6f;
        anim.AddFrame(frame);
        frame.timeStamp = 0.99f;
        anim.AddFrame(frame);

        sut_.SetAnimation(CLIP_NAME);
    }
    AnimatorTestWrapper sut_;
};

TEST_F(AnimatorTestSchould, GetLastNextFrame)
{
    sut_.SetTime(.5f);
    auto result = sut_._getPreviousAndNextFrames();
    ASSERT_FLOAT_EQ(result.first.timeStamp, 0.33f);
    ASSERT_FLOAT_EQ(result.second.timeStamp, 0.6f);
}
