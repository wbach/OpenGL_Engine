#include <Utils/Time/Timer.h>
#include <gtest/gtest.h>

#include "../BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "Logger/Log.h"

using namespace GameEngine;
using namespace GameEngine::Animation;
using namespace GameEngine::Components;
using namespace ::testing;

namespace
{
const std::string CLIP_NAME{"DefaultAnimationClip"};
const std::string CLIP_NAME2{"DefaultAnimationClip2"};
const std::string ROOT_MONTION_CLIP{"RootMontionAnimationClip"};
const uint32 BONE_COUNT{100};
const std::string boneName{"bone_"};
const int FRAMES_COUNT{4};
const float FRAMES_DELTA{0.33f};
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
        createClip(ROOT_MONTION_CLIP, true);
    }
    void createClip(const std::string& clipName, AnimationClipInfo::UseRootMontion rootMontion = false)
    {
        AnimationClip clip(clipName);
        KeyFrame frame;

        for (auto i = 0u; i < BONE_COUNT; ++i)
        {
            frame.transforms.insert({i, JointTransform{}});
        }

        for (int i = 0; i < FRAMES_COUNT; i++)
        {
            frame.timeStamp              = static_cast<float>(i) * FRAMES_DELTA;
            frame.transforms[1].position = vec3(static_cast<float>(i), 0.f, 0.f);
            clip.AddFrame(frame);
        }

        //        frame.timeStamp              = 0;
        //        frame.transforms[0].position = vec3(0.0f, 0.f, 0.f);
        //        clip.AddFrame(frame);
        //        frame.timeStamp              = 0.33f;
        //        frame.transforms[0].position = vec3(1.0f, 0.f, 0.f);
        //        clip.AddFrame(frame);
        //        frame.timeStamp              = 0.6f;
        //        frame.transforms[0].position = vec3(2.0f, 0.f, 0.f);
        //        clip.AddFrame(frame);
        //        frame.timeStamp              = 0.99f;
        //        frame.transforms[0].position = vec3(3.0f, 0.f, 0.f);
        //        clip.AddFrame(frame);

        sut_.AddAnimationClip(clipName, clip, AnimationClipInfo::PlayType::loop, rootMontion);
    }

    AnimatorTestWrapper sut_;
};

TEST_F(AnimatorTestSchould, GetLastNextFrame)
{
    float currentTime{.5f};
    sut_.setDeltaTime(currentTime);
    const auto& anim            = sut_.getAnimationClips().at(CLIP_NAME).clip;
    auto [prevFrame, nextFrame] = getPreviousAndNextFrames(anim, currentTime);
    ASSERT_FLOAT_EQ(prevFrame->timeStamp.value, 0.33f);
    ASSERT_FLOAT_EQ(nextFrame->timeStamp.value, 0.66f);
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
                                        LOG_DEBUG << "Execute time : " << time;
                                    });

    LOG_DEBUG << "Start timer...";

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
    LOG_DEBUG << "Avarage frame time : " << static_cast<double>(avarageFrameTime) / static_cast<double>(frameCounter);
    LOG_DEBUG << "Avarage animation time : " << static_cast<double>(avarageTime) / static_cast<double>(repeatCount);
}

TEST_F(AnimatorTestSchould, DISABLED_rootMontion)
{
    auto rootJoint = sut_.GetRootJoint();
    EXPECT_TRUE(rootJoint);
    LOG_DEBUG << "RootJointName: " + rootJoint->name;
    sut_.SetAnimation(ROOT_MONTION_CLIP);

    LOG_DEBUG << "Update deltaTime: " << DEFAULT_ANIMATION_TRANSITION_TIME;
    context_.time_.deltaTime = DEFAULT_ANIMATION_TRANSITION_TIME;
    // componentController_.CallFunctions(FunctionType::Update);
    sut_.Update();
    sut_.Update();
    for (int frame = 0; frame < FRAMES_COUNT; frame++)
    {
        context_.time_.deltaTime = FRAMES_DELTA;
        sut_.Update();
        LOG_DEBUG << obj_.GetWorldTransform().GetPosition();
    }
}
