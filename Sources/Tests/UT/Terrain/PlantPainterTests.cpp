#include <GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h>
#include <gtest/gtest.h>
#include <math.h>

#include <functional>
#include <limits>
#include <memory>
#include <vector>

#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Painter/CircleBrush.h"
#include "GameEngine/DebugTools/Painter/Interpolation.h"
#include "GameEngine/DebugTools/Painter/PlantPainter.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/File.h"
#include "IThreadSync.h"
#include "IdPool.h"
#include "Input/KeyCodes.h"
#include "Logger/Log.h"
#include "Tests/UT/EngineBasedTest.h"
#include "Types.h"
#include "gmock/gmock.h"

using namespace ::testing;

namespace
{
constexpr uint32 HEIGHTMAP_SIZE = 101;
const vec3 TERRAIN_SCALE{100.f, 50.f, 100.f};
const vec3 TERRAIN_POSITION{vec3(20.f, 10.f, 15.f)};
const float WORLD_RADIOUS{3.f};
};  // namespace

struct PlantPainterShould : public EngineBasedTest
{
    void SetUp() override
    {
        EngineBasedTest::SetUp();

        Utils::Image image;
        image.width  = HEIGHTMAP_SIZE;
        image.height = HEIGHTMAP_SIZE;
        image.setChannels(1);
        image.allocateImage<float>();
        heightmap_ = std::make_unique<HeightMap>(*graphicsApi, GameEngine::TextureParameters(), "file", std::move(image));

        PrepareTerrainComponent();
        LOG_DEBUG << "Setup done";
        CreatePainter(1.f);
    }

    void TearDown() override
    {
    }

    void CreatePainter(float strength)
    {
        EXPECT_CALL(*threadSync, Subscribe(_, _, _))
            .WillRepeatedly(
                [&](auto func, const auto& label, auto) -> IdType
                {
                    LOG_DEBUG << "Subscribe thread : " << label;
                    threads_.insert({label, func});
                    return idPool_.getId();
                });

        auto& camera = *scene->GetCameraManager().GetMainCamera();
        PlantPainter::Dependencies parameters{.inputManager        = engineContext->GetInputManager(),
                                              .threadSync          = engineContext->GetThreadSync(),
                                              .camera              = camera,
                                              .componentController = scene->getComponentController(),
                                              .scene               = *scene};

        auto circleBrush = std::make_unique<GameEngine::CircleBrush>(GameEngine::makeInterpolation(InterpolationType::Linear),
                                                                     GameEngine::WorldSpaceBrushRadius{WORLD_RADIOUS}, strength);
        brush_           = circleBrush.get();
        sut_             = std::make_unique<PlantPainter>(std::move(parameters), selectedPlantTexture, std::move(circleBrush),
                                              PlantPainter::PaintMode::Terrain, baseColor, colorRandomness, sizeRandomness,
                                              density, randomness);

        sut_->Start();
    }

    void PrepareTerrainComponent()
    {
        EXPECT_CALL(*graphicsApi, IsTesselationSupported()).WillRepeatedly(Return(false));
        EXPECT_CALL(*textureLoader, CreateHeightMap(_, _, _)).WillRepeatedly(Return(heightmap_.get()));

        auto terrainGo = scene->CreateGameObject("Terrain");
        terrainGo->SetLocalScale(TERRAIN_SCALE);
        terrainGo->SetLocalPosition(TERRAIN_POSITION);
        terrainGameObject = terrainGo.get();
        LOG_DEBUG << "Add TerrainRendererComponent";
        auto& tc = terrainGo->AddComponent<Components::TerrainRendererComponent>();

        EXPECT_CALL(*textureLoader, GetGraphicsApi()).WillRepeatedly(ReturnRef(*graphicsApi));
        LOG_DEBUG << "Add createHeightMap";
        tc.createHeightMap();
        LOG_DEBUG << "AddGameObject";
        scene->AddGameObject(std::move(terrainGo));
        scene->ProcessEvents();
        LOG_DEBUG << "Return world pos : " << terrainGameObject->GetWorldTransform().GetPosition();
    }

    std::unique_ptr<HeightMap> heightmap_;
    std::unique_ptr<Painter> sut_;
    std::unordered_map<std::string, Utils::Thread::FrameFunc> threads_;
    Utils::IdPool idPool_;
    CircleBrush* brush_;
    GameEngine::File selectedPlantTexture{"grass.png"};
    float density{1.f};
    float randomness{1.f};
    GameObject* terrainGameObject;
    vec3 colorRandomness{vec3(0.0f)};
    float sizeRandomness{0.0f};
    Color baseColor{vec4(1.f)};
};

TEST_F(PlantPainterShould, noPaintWithoutClick)
{
    auto& camera = *scene->GetCameraManager().GetMainCamera();
    camera.SetPosition(vec3(0, 1, 0));
    camera.LookAt(vec3(0, -1, 0));
    camera.UpdateMatrix();

    threads_.at(PAINTER_THREAD_NAME);
    sut_.reset();
}

TEST_F(PlantPainterShould, PaintInTheMiddle)
{
    auto& camera = *scene->GetCameraManager().GetMainCamera();
    camera.SetPosition(TERRAIN_POSITION + vec3(0, 1, 0));
    camera.LookAt(TERRAIN_POSITION + vec3(0, -1, 0));
    camera.UpdateMatrix();

    EXPECT_CALL(inputManagerMock, GetMousePosition()).WillRepeatedly(Return(vec2(0.f)));
    EXPECT_CALL(inputManagerMock, GetMouseKey(KeyCodes::LMOUSE)).WillRepeatedly(Return(true));
    const float deltaTime = 1.0f;
    threads_.at(PAINTER_THREAD_NAME)(deltaTime);

    // GrassRendererComponent should be implicit added
    auto plantComponent = terrainGameObject->GetComponent<GameEngine::Components::GrassRendererComponent>();
    EXPECT_NE(plantComponent, nullptr);
    EXPECT_EQ(plantComponent->textureFile, selectedPlantTexture);
    EXPECT_FALSE(plantComponent->dataFile.empty());

    bool centralPointFound = false;
    for (auto& ssbo : plantComponent->GetSsbo()->GetData())
    {
        auto& v = ssbo.position.value;
        if (glm::abs(v.x - TERRAIN_POSITION.x) < randomness and glm::abs(v.y - TERRAIN_POSITION.y) < 0.01f and
            glm::abs(v.z - TERRAIN_POSITION.z) < randomness)
        {
            centralPointFound = true;
        }
    }

    EXPECT_TRUE(centralPointFound);

    LOG_DEBUG << "meshes.positions.size() = " << plantComponent->GetSsbo()->GetData().size();

    sut_.reset();
}
