#include "GameEngineTests/Tests/UT/Components/BaseComponent.h"
#include "GameEngine/Components/Renderer/ParticleEffectComponent.h"

using namespace GameEngine;
using namespace GameEngine::Components;

using namespace testing;

struct ParicleComponentTestSchould : public BaseComponentTestSchould
{
	ParicleComponentTestSchould()
	{
		Init(&sut_);
	}
	virtual void SetUp() override
	{

	}

	ParticleEffectComponent sut_;
};


TEST_F(ParicleComponentTestSchould, GetLastNextFrame)
{
	time_.deltaTime = 0.1f;
	sut_.ReqisterFunctions();
	vec3 camPosition(0);
	EXPECT_CALL(*camera_, GetPosition()).WillRepeatedly(ReturnRef(camPosition));
	componentController_.Update();
	EXPECT_TRUE(true);
}
