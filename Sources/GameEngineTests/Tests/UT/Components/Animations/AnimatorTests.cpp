#include <Utils/Time/Timer.h>
#include <gtest/gtest.h>

#include "../BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"

using namespace GameEngine;
using namespace GameEngine::Animation;
using namespace GameEngine::Components;
using namespace ::testing;

namespace
{
const std::string CLIP_NAME{"DefaultAnimationClip"};
const std::string CLIP_NAME2{"DefaultAnimationClip2"};
const uint32 BONE_COUNT{100};
const std::string boneName{"bone_"};
}  // namespace

struct AnimatorTestWrapper : public Animator
{
    AnimatorTestWrapper(ComponentContext& context, GameObject& go)
        : Animator(context, go)
    {
        rootJoint = &jointData_.rootJoint;

        jointData_.rootJoint.id   = 0;
        jointData_.rootJoint.name = boneName + std::to_string(jointData_.rootJoint.id);

        for (auto i = 1u; i < BONE_COUNT - 1; ++i)
        {
            Joint joint;
            joint.id   = i;
            joint.name = boneName + std::to_string(joint.id);
            jointData_.rootJoint.addChild(joint);
        }
    }
    std::pair<KeyFrame, KeyFrame> _getPreviousAndNextFrames()
    {
        return {};
    }
    void setDeltaTime(float t)
    {
        componentContext_.time_.deltaTime = t;
    }

    Joint* rootJoint{nullptr};
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
        sut_.setDeltaTime(.01f);
        createClip(CLIP_NAME);
        createClip(CLIP_NAME2);
    }
    void createClip(const std::string& clipName)
    {
        AnimationClip clip(clipName);
        KeyFrame frame;

        for (auto i = 0u; i < BONE_COUNT; ++i)
        {
            frame.transforms.insert({i, JointTransform{}});
        }

        frame.timeStamp = 0;
        clip.AddFrame(frame);
        frame.timeStamp = 0.33f;
        clip.AddFrame(frame);
        frame.timeStamp = 0.6f;
        clip.AddFrame(frame);
        frame.timeStamp = 0.99f;
        clip.AddFrame(frame);

        sut_.AddAnimationClip(clipName, clip);
        sut_.SetAnimation(clipName);
    }

    AnimatorTestWrapper sut_;
};

TEST_F(AnimatorTestSchould, GetLastNextFrame)
{
    float currentTime{.5f};
    sut_.setDeltaTime(currentTime);
    const auto& anim  = sut_.getAnimationClips().at(CLIP_NAME).clip;
    auto [prevFrame, nextFrame] = getPreviousAndNextFrames(anim, currentTime);
    ASSERT_FLOAT_EQ(prevFrame->timeStamp.value, 0.33f);
    ASSERT_FLOAT_EQ(nextFrame->timeStamp.value, 0.6f);
}

TEST_F(AnimatorTestSchould, FullUpdateOneCycle)
{
    Utils::Timer timer;
    Utils::Timer frameTimer;
    bool run{true};
    uint64 avarageTime{0};
    uint64 avarageFrameTime{0};
    sut_.SubscribeForAnimationFrame(CLIP_NAME,
                                    [&timer, &run, &avarageTime]()
                                    {
                                        run       = false;
                                        auto time = timer.GetTimeNanoseconds();
                                        avarageTime += time;
                                        DEBUG_LOG("Execute time : " + std::to_string(time));
                                    });

    DEBUG_LOG("Start timer...");

    int repeatCount{30};
    int frameCounter{0};
    for (int i = 0; i < repeatCount; ++i)
    {
        run = true;
        sut_.ChangeAnimation(CLIP_NAME, Animator::AnimationChangeType::smooth);
        timer.Reset();
        while (run)
        {
            frameTimer.Reset();
            sut_.Update();
            avarageFrameTime += frameTimer.GetTimeNanoseconds();
            ++frameCounter;
        }
    }
    DEBUG_LOG("Avarage frame time : " +
              std::to_string(static_cast<double>(avarageFrameTime) / static_cast<double>(frameCounter)));
    DEBUG_LOG("Avarage animation time : " +
              std::to_string(static_cast<double>(avarageTime) / static_cast<double>(repeatCount)));
}

TEST_F(AnimatorTestSchould, StateMachinePlayToPlay)
{
}
