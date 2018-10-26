#include "GameEngine/Components/Renderer/ParticleEffectComponent.h"
#include "GameEngineTests/Tests/UT/Components/BaseComponent.h"

using namespace GameEngine;
using namespace GameEngine::Components;

using namespace testing;

const uint32 PARTICLES_PER_SEC = 100;
const float DELTA_TIME         = 0.1f;

struct ParicleComponentTestSchould : public BaseComponentTestSchould
{
    ParicleComponentTestSchould()
    {
        Init(&sut_);
    }
    virtual void SetUp() override
    {
        sut_.SetParticlesLimit(1000);
        sut_.SetEmitFunction(std::bind(&ParicleComponentTestSchould::Emit, this, std::placeholders::_1));
    }
    GameEngine::Particle Emit(const GameEngine::Particle& p)
    {
        return EmitParticle(p);
    }
    MOCK_METHOD1(EmitParticle, GameEngine::Particle(const GameEngine::Particle&));
    ParticleEffectComponent sut_;
};

TEST_F(ParicleComponentTestSchould, EmitParticlesCountTest)
{
    time_.deltaTime = 0.1f;
    sut_.SetParticlesPerSec(100);
    sut_.ReqisterFunctions();
    vec3 camPosition(0);
    GameEngine::Particle particle;
    EXPECT_CALL(*camera_, GetPosition()).WillRepeatedly(Return(camPosition));
    EXPECT_CALL(*this, EmitParticle(_)).WillRepeatedly(Return(particle));
    componentController_.Update();
    EXPECT_EQ(sut_.GetParticlesCount(), 10);
}
