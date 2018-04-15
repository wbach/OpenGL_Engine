#include "GameEngineTests/Tests/UT/Components/BaseComponent.h"
#include "GameEngine/Components/Renderer/ParticleEffectComponent.h"

using namespace GameEngine;
using namespace GameEngine::Components;

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
	sut_.ReqisterFunctions();
	componentController_.Update();
	EXPECT_TRUE(true);
}
