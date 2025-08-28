#include "GameEngine/Components/Renderer/Particles/ParticleEffectComponent.h"
#include "Tests/UT/Components/BaseComponent.h"

using namespace GameEngine;
using namespace GameEngine::Components;

using namespace testing;

const uint32 PARTICLES_PER_SEC = 100;
const float DELTA_TIME         = 0.1f;

struct ParicleComponentTestSchould : public BaseComponentTestSchould
{
    ParicleComponentTestSchould()
        : sut_(context_, obj_)
    {
    }
    void SetUp() override
    {
        sut_.SetParticlesLimit(1000);
        sut_.SetEmitFunction(std::bind(&ParicleComponentTestSchould::Emit, this, std::placeholders::_1));
    }
    GameEngine::Particle Emit(const GameEngine::Particle& p) const
    {
        return EmitParticle(p);
    }
    MOCK_CONST_METHOD1(EmitParticle, GameEngine::Particle(const GameEngine::Particle&));
    ParticleEffectComponent sut_;
};

TEST_F(ParicleComponentTestSchould, EmitParticlesCountTest)
{
    vec3 camPosition(0);
    EXPECT_CALL(cameraMock_, GetPosition()).WillRepeatedly(ReturnRef(camPosition));
    GameEngine::Particle particle;
    EXPECT_CALL(*this, EmitParticle(_)).WillRepeatedly(Return(particle));

    time_.deltaTime = 0.1f;
    sut_.SetParticlesPerSec(100);
    sut_.ReqisterFunctions();
    componentController_.Update();
    EXPECT_EQ(sut_.GetParticlesCount(), 10);
}
