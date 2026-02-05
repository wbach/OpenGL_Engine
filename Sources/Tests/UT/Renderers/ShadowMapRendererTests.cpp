#include <GameEngine/Components/Lights/DirectionalLightComponent.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/Objects/Shadows/ShadowBox.h>
#include <gtest/gtest.h>

#include "Camera/Camera.h"
#include "Engine/ConfigurationParams/RendererParams/ShadowsParams/ShadowsParams.h"
#include "Logger/Log.h"

namespace
{
struct EngineConfSetup
{
    EngineConfSetup()
    {
        EngineConf.renderer.shadows.cascadeDistanceFunc  = GameEngine::Params::Shadows::CascadeDistanceFunc::linear;
        EngineConf.renderer.shadows.firstCascadeDistance = 25.f;
        EngineConf.renderer.shadows.distance             = 100.f;
        EngineConf.renderer.shadows.cascadesSize         = 4;
    }
};

}  // namespace

class ShadowBoxTests : public ::testing::Test
{
public:
    ShadowBoxTests()
    {
    }

    EngineConfSetup config;

    GameEngine::Camera camera;
    vec3 lightDir;
    GameEngine::ShadowBox sut;
};

TEST_F(ShadowBoxTests, calculateMatrixes)
{
    sut.update(camera, lightDir);

    LOG_DEBUG << "Test done";
}
