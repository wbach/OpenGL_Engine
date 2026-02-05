#include <GameEngine/Renderers/Objects/Shadows/ShadowBox.h>
#include <GameEngine/Components/Lights/DirectionalLightComponent.h>
#include <gtest/gtest.h>

#include "Camera/Camera.h"

class ShadowBoxTests : public ::testing::Test
{
public:
    GameEngine::Camera camera;
    vec3 lightDir;

    GameEngine::ShadowBox sut;
};

TEST_F(ShadowBoxTests, calculateMatrixes)
{
    sut.update(camera, lightDir);
}