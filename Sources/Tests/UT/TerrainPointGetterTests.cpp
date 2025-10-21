#include <GameEngine/Camera/CameraWrapper.h>
#include <GameEngine/Camera/ICamera.h>
#include <GameEngine/Components/ComponentController.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h>
#include <GameEngine/DebugTools/Painter/TerrainPoint.h>
#include <GameEngine/DebugTools/Painter/TerrainPointGetter.h>
#include <GameEngine/Renderers/Projection.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <Logger/Log.h>
#include <gtest/gtest.h>

#include <memory>

#include "Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "Renderers/Projection.h"
#include "Tests/Mocks/Api/GraphicsApiMock.h"
#include "Tests/UT/EngineBasedTest.h"
#include "gmock/gmock.h"

using namespace ::testing;

namespace GameEngine
{
struct TerrainPointGetterTestParam
{
    vec3 cameraPos;
    vec3 lookAt;
    std::optional<vec3> expectedPoint;
    vec2 mousePos;
    std::string name;
};

// Matcher dla pojedynczego glm::vec3 z tolerancją
MATCHER_P2(Vec3Near, expected, eps, "Sprawdza czy dwa glm::vec3 są blisko siebie z tolerancją " + std::to_string(eps))
{
    if (!::glm::all(::glm::lessThanEqual(::glm::abs(arg - expected), glm::vec3(eps))))
    {
        *result_listener << "Różnica: (" << arg.x - expected.x << ", " << arg.y - expected.y << ", " << arg.z - expected.z << ")";
        return false;
    }
    return true;
}

// Matcher dla std::optional<glm::vec3>
MATCHER_P2(OptionalVec3Near, expectedOpt, eps, "Porównuje std::optional<glm::vec3> z tolerancją " + std::to_string(eps))
{
    if (arg.has_value() != expectedOpt.has_value())
    {
        *result_listener << "Jedno optional ma wartość, drugie nie";
        return false;
    }
    if (!arg.has_value())
        return true;  // oba nullopt → ok

    return ExplainMatchResult(Vec3Near(*expectedOpt, eps), *arg, result_listener);
}

struct TerrainPointGetterShould : public EngineBasedTest, public ::testing::WithParamInterface<TerrainPointGetterTestParam>
{
    TerrainPointGetterShould()
    {
        uint32 heightMapSize = 100;
        Utils::Image image;
        image.width  = heightMapSize;
        image.height = heightMapSize;
        image.setChannels(1);
        image.allocateImage<float>();
        heightmap_ = std::make_unique<HeightMap>(*graphicsApi, GameEngine::TextureParameters(), "file", std::move(image));

        heightmap_->SetHeight(vec2ui(0, 0), 10.f);
        heightmap_->SetHeight(vec2ui(0, heightMapSize - 1), 10.f);
        heightmap_->SetHeight(vec2ui(heightMapSize - 1, 0), 10.f);
        heightmap_->SetHeight(vec2ui(heightMapSize - 1, heightMapSize - 1), 10.f);
    }
    void SetUp() override
    {
        EngineBasedTest::SetUp();
        sut_ = std::make_unique<TerrainPointGetter>(scene->GetCamera(), engineContext->GetRenderersManager().GetProjection(),
                                                    engineContext->GetDisplayManager().GetWindowSize(),
                                                    scene->getComponentController());
    }
    void TearDown() override
    {
    }

    void PrepareTerrainComponent()
    {
        EXPECT_CALL(*graphicsApi, IsTesselationSupported()).WillRepeatedly(Return(false));
        EXPECT_CALL(*textureLoader, CreateHeightMap(_, _, _)).WillRepeatedly(Return(heightmap_.get()));

        auto terrainGo = scene->CreateGameObject("Terrain");
        terrainGo->SetLocalScale(vec3(100, 100, 100));
        LOG_DEBUG << "Add TerrainRendererComponent";
        auto& tc = terrainGo->AddComponent<Components::TerrainRendererComponent>();

        EXPECT_CALL(*textureLoader, GetGraphicsApi()).WillRepeatedly(ReturnRef(*graphicsApi));
        LOG_DEBUG << "Add createHeightMap";
        tc.createHeightMap();
        LOG_DEBUG << "AddGameObject";
        scene->AddGameObject(std::move(terrainGo));
        LOG_DEBUG << "Return";
    }

    std::unique_ptr<HeightMap> heightmap_;
    std::unique_ptr<TerrainPointGetter> sut_;
};

TEST_F(TerrainPointGetterShould, noTerrainExist)
{
    auto& camera = scene->GetCamera();
    camera.SetPosition(vec3(0, 1, 0));
    camera.LookAt(vec3(0, -1, 0));
    camera.UpdateMatrix();

    auto maybePoint = sut_->GetMousePointOnTerrain(vec2(0, 0));
    EXPECT_FALSE(maybePoint.has_value());
}

TEST_F(TerrainPointGetterShould, oneTerrainExist)
{
    LOG_DEBUG << "oneTerrainExist start";

    PrepareTerrainComponent();

    auto& camera = scene->GetCamera();
    camera.SetPosition(vec3(0, 1, 0));
    camera.LookAt(vec3(0, -1, 0));
    camera.UpdateMatrix();

    auto maybePoint = sut_->GetMousePointOnTerrain(vec2(0, 0));
    LOG_DEBUG << "maybePoint: " << maybePoint;

    EXPECT_TRUE(maybePoint.has_value());
    EXPECT_FLOAT_EQ(maybePoint->pointOnTerrain.x, 0.f);
    EXPECT_FLOAT_EQ(maybePoint->pointOnTerrain.y, 0.f);
    EXPECT_FLOAT_EQ(maybePoint->pointOnTerrain.z, 0.f);
}

TEST_P(TerrainPointGetterShould, DetectTerrainPointCorrectly)
{
    const auto& param = GetParam();

    PrepareTerrainComponent();

    auto& camera = scene->GetCamera();

    camera.SetPosition(param.cameraPos);
    camera.LookAt(param.lookAt);
    camera.UpdateMatrix();

    auto maybePoint = sut_->GetMousePointOnTerrain(param.mousePos);
    LOG_DEBUG << "maybePoint: " << maybePoint;

    EXPECT_THAT(maybePoint ? std::optional{maybePoint->pointOnTerrain} : std::nullopt,
                OptionalVec3Near(param.expectedPoint, 0.001f));
}

INSTANTIATE_TEST_SUITE_P(TerrainCases, TerrainPointGetterShould,
                         ::testing::Values(TerrainPointGetterTestParam{.cameraPos     = vec3(0, 1, 0),
                                                                       .lookAt        = vec3(0, -1, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "CameraAboveLookingDown"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(2, 2, 2),
                                                                       .lookAt        = vec3(0, 0, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "DiagonalView"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(0, 10, 0),
                                                                       .lookAt        = vec3(0, 0, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "HighAboveLookingDown"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(0, -10, 0),
                                                                       .lookAt        = vec3(0, 0, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "BelowTerrainLookingUp"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(10, 10, 10),
                                                                       .lookAt        = vec3(0, 0, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "Diagonal_NW"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(-10, 10, 10),
                                                                       .lookAt        = vec3(0, 0, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "Diagonal_NE"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(10, 10, -10),
                                                                       .lookAt        = vec3(0, 0, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "Diagonal_SW"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(-10, 10, -10),
                                                                       .lookAt        = vec3(0, 0, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "Diagonal_SE"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(0, 0.5f, -5),
                                                                       .lookAt        = vec3(0, 0, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "GroundLevel_LookingForward"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(0, 1, -5),
                                                                       .lookAt        = vec3(0, 1, 0),
                                                                       .expectedPoint = std::nullopt,
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "ParallelToGround_NoIntersection"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(200, 50, 200),
                                                                       .lookAt        = vec3(0, 0, 0),
                                                                       .expectedPoint = vec3(0, 0, 0),
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "FarOutsideLookingIn"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(0, 10, 0),
                                                                       .lookAt        = vec3(0, 20, 0),
                                                                       .expectedPoint = std::nullopt,
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "LookingUp_NoHit"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(0, 100, 0),
                                                                       .lookAt        = vec3(-51.1f, 0, -51.1f),
                                                                       .expectedPoint = std::nullopt,
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "Miss_Corner_LowerLeft"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(0, 100, 0),
                                                                       .lookAt        = vec3(-51.1f, 0, 51.1f),
                                                                       .expectedPoint = std::nullopt,
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "Miss_Corner_UpperLeft"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(0, 100, 0),
                                                                       .lookAt        = vec3(51.1f, 0, -51.1f),
                                                                       .expectedPoint = std::nullopt,
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "Miss_Corner_LowerRight"},
                                           TerrainPointGetterTestParam{.cameraPos     = vec3(0, 100, 0),
                                                                       .lookAt        = vec3(51.1f, 0, 51.1f),
                                                                       .expectedPoint = std::nullopt,
                                                                       .mousePos      = vec2(0, 0),
                                                                       .name          = "Miss_Corner_UpperRight"}),

                         [](const testing::TestParamInfo<TerrainPointGetterTestParam>& info) { return info.param.name; });
}  // namespace GameEngine
