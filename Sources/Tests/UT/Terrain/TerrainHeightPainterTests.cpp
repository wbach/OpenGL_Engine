 #include <GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h>

#include <functional>
#include <limits>
#include <memory>
#include <vector>

#include "Components/Renderer/Terrain/TerrainHeightTools.h"
#include "Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "DebugTools/Painter/CircleBrush.h"
#include "DebugTools/Painter/HeightPainter.h"
#include "DebugTools/Painter/Interpolation.h"
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

struct TerrainHeightPainterShould : public EngineBasedTest
{
    TerrainHeightPainterShould()
    {
        Utils::Image image;
        image.width  = HEIGHTMAP_SIZE;
        image.height = HEIGHTMAP_SIZE;
        image.setChannels(1);
        image.allocateImage<float>();
        heightmap_ = std::make_unique<HeightMap>(*graphicsApi, GameEngine::TextureParameters(), "file", std::move(image));
    }
    void SetUp() override
    {
        EngineBasedTest::SetUp();
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

        HeightPainter::Dependencies parameters{
            .inputManager        = engineContext->GetInputManager(),
            .threadSync          = engineContext->GetThreadSync(),
            .camera              = scene->GetCamera(),
            .projection          = engineContext->GetRenderersManager().GetProjection(),
            .componentController = scene->getComponentController(),
        };

        auto circleBrush = std::make_unique<GameEngine::CircleBrush>(GameEngine::makeInterpolation(InterpolationType::Gaussian),
                                                                     GameEngine::WorldSpaceBrushRadius{WORLD_RADIOUS}, strength);
        brush_           = circleBrush.get();
        sut_             = std::make_unique<HeightPainter>(std::move(parameters), std::move(circleBrush));
        sut_->Start();
    }

    void PrepareTerrainComponent()
    {
        EXPECT_CALL(*graphicsApi, IsTesselationSupported()).WillRepeatedly(Return(false));
        EXPECT_CALL(*textureLoader, CreateHeightMap(_, _, _)).WillRepeatedly(Return(heightmap_.get()));

        auto terrainGo = scene->CreateGameObject("Terrain");
        terrainGo->SetLocalScale(TERRAIN_SCALE);
        terrainGo->SetLocalPosition(TERRAIN_POSITION);
        auto ptr = terrainGo.get();
        LOG_DEBUG << "Add TerrainRendererComponent";
        auto& tc = terrainGo->AddComponent<Components::TerrainRendererComponent>();

        EXPECT_CALL(*textureLoader, GetGraphicsApi()).WillRepeatedly(ReturnRef(*graphicsApi));
        LOG_DEBUG << "Add createHeightMap";
        tc.createHeightMap();
        LOG_DEBUG << "AddGameObject";
        scene->AddGameObject(std::move(terrainGo));
        scene->ProcessEvents();
        LOG_DEBUG << "Return world pos : " << ptr->GetWorldTransform().GetPosition();
    }

    std::unique_ptr<HeightMap> heightmap_;
    std::unique_ptr<Painter> sut_;
    std::unordered_map<std::string, Utils::Thread::FrameFunc> threads_;
    Utils::IdPool idPool_;
    CircleBrush* brush_;
};

TEST_F(TerrainHeightPainterShould, noPaintWithoutClick)
{
    auto& camera = scene->GetCamera();
    camera.SetPosition(vec3(0, 1, 0));
    camera.LookAt(vec3(0, -1, 0));
    camera.UpdateMatrix();

    threads_.at(PAINTER_THREAD_NAME);
    sut_.reset();
}

void visualizeBrushInWorld(const CircleBrush& brush, const vec3& terrainPosition, const vec3& terrainScale)
{
    const auto& influence = brush.getInfluence();
    if (influence.empty())
    {
        LOG_DEBUG << "No influence points to visualize";
        return;
    }

    LOG_DEBUG << "World-space brush visualization:";

    for (const auto& point : influence)
    {
        // Local brush point do ujęcia w pixel space
        float u = static_cast<float>(point.point.x) / brush.getLocalBrushSize().value;
        float v = static_cast<float>(point.point.y) / brush.getLocalBrushSize().value;

        // Skalowanie do world space
        float worldX = terrainPosition.x + u * terrainScale.x - terrainScale.x / 2.0f;
        float worldZ = terrainPosition.z + v * terrainScale.z - terrainScale.z / 2.0f;
        float worldY = terrainPosition.y;

        LOG_DEBUG << "World point: (" << worldX << ", " << worldY << ", " << worldZ << ") intensity=" << point.influance;
    }
}

void visualizeBrushWorldHeights(const CircleBrush& brush, const vec3& terrainCenter, const TerrainHeightGetter& getter,
                                float worldRadius,
                                float step = 0.5f)
{
    LOG_DEBUG << "Visualizing brush (radius=" << worldRadius << ") around center: (" << terrainCenter.x << ", " << terrainCenter.y
              << ", " << terrainCenter.z << ")";

    std::vector<std::string> lines;

    float max = -std::numeric_limits<float>::max();
    vec2 pos;
    for (float dz = -worldRadius - step; dz <= worldRadius + step; dz += step)
    {
        std::stringstream line;
        for (float dx = -worldRadius - step; dx <= worldRadius + step; dx += step)
        {
            float worldX = terrainCenter.x + dx;
            float worldZ = terrainCenter.z + dz;

            auto heightOpt = getter.GetHeightofTerrain(worldX, worldZ);

            if (heightOpt)
            {
                if (max < heightOpt.value())
                {
                    max = heightOpt.value();
                    pos = vec2{worldX, worldZ};
                }
                line << std::fixed << std::setprecision(2) << heightOpt.value() << " ";
            }
            else
            {
                line << "N/A ";
            }
        }
        lines.push_back(line.str());
    }

    LOG_DEBUG << "Max value = " << max << " world pos: " << pos;

    LOG_DEBUG << "Terrain visualization: ";
    for (const auto& line : lines)
        LOG_DEBUG_RAW << line;
}

TEST_F(TerrainHeightPainterShould, PaintInTheMiddle)
{
    auto& camera = scene->GetCamera();
    camera.SetPosition(TERRAIN_POSITION + vec3(0, 1, 0));
    camera.LookAt(TERRAIN_POSITION + vec3(0, -1, 0));
    camera.UpdateMatrix();

    EXPECT_CALL(inputManagerMock, GetMousePosition()).WillRepeatedly(Return(vec2(0.f)));
    EXPECT_CALL(inputManagerMock, GetMouseKey(KeyCodes::LMOUSE)).WillRepeatedly(Return(true));
    const float deltaTime = 1.0f;
    threads_.at(PAINTER_THREAD_NAME)(deltaTime);

    auto maybeHeightFromHeightMap = heightmap_->GetHeight(vec2ui(HEIGHTMAP_SIZE / 2));
    EXPECT_NEAR(maybeHeightFromHeightMap.value(), 1.f, 0.01f);

    LOG_DEBUG << "Check world height";
    GameEngine::TerrainHeightGetter getter(TERRAIN_SCALE, *heightmap_, TERRAIN_POSITION);
    auto maybeHeight = getter.GetHeightofTerrain(TERRAIN_POSITION.x, TERRAIN_POSITION.z);
    visualizeBrushWorldHeights(*brush_, TERRAIN_POSITION, getter, WORLD_RADIOUS);

    EXPECT_NEAR(maybeHeight.value(), TERRAIN_SCALE.y + TERRAIN_POSITION.y, 0.01f);
    sut_.reset();
}
