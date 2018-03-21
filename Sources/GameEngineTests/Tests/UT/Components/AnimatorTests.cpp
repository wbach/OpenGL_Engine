#include <gtest/gtest.h>
#include "GameEngine/Components/Animation/Animator.h"

using namespace GameEngine;
using namespace GameEngine::Animation;
using namespace GameEngine::Components;


struct AnimatorTestWrapper : public Animator
{
	std::pair<KeyFrame, KeyFrame> _getPreviousAndNextFrames()
	{
		return getPreviousAndNextFrames();
	}
	void SetTime(float t)
	{
		currentTime_ = t;
	}
};

struct AnimatorTestSchould : public ::testing::Test
{

	virtual void SetUp() override
	{
		auto& anim = sut_.animationClips_["DefaultAnimationClip"];
		anim.name = "DefaultAnimationClip";
		
		KeyFrame frame;
		frame.timeStamp = 0;
		anim.AddFrame(frame);
		frame.timeStamp = 0.33f;
		anim.AddFrame(frame);
		frame.timeStamp = 0.6f;
		anim.AddFrame(frame);
		frame.timeStamp = 0.99f;
		anim.AddFrame(frame);
	}
	AnimatorTestWrapper sut_;
};


TEST_F(AnimatorTestSchould, GetLastNextFrame)
{
	sut_.SetTime(.5f);
	auto result  = sut_._getPreviousAndNextFrames();
	ASSERT_FLOAT_EQ(result.first.timeStamp, 0.33f);
	ASSERT_FLOAT_EQ(result.second.timeStamp, 0.6f);
}
