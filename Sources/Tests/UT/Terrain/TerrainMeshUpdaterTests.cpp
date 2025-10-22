#include <GameEngine/Components/Renderer/Terrain/TerrainMeshUpdater.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h>

#include <memory>

#include "Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "Logger/Log.h"
#include "Tests/UT/EngineBasedTest.h"
#include "Types.h"
#include "gtest/gtest.h"

using namespace GameEngine;
using namespace GameEngine::Components;
using namespace ::testing;

namespace
{
const uint32 heightMapSize = 100;
}
class TerrainMeshUpdaterTest : public EngineBasedTest
{
protected:
    void SetUp() override
    {
        EngineConf.renderer.terrain.meshPartsCount = 4;

        EngineBasedTest::SetUp();

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

        PrepareTerrainComponent();

        TerrainMeshUpdater::EntryParameters params{
            *graphicsApi,     engineContext->GetGpuResourceLoader(), scene->GetResourceManager(), *model, *heightmap_,
            worldTerrainScale};

        sut_ = std::make_unique<TerrainMeshUpdater>(params);
    }

    void PrepareTerrainComponent()
    {
        EXPECT_CALL(*graphicsApi, IsTesselationSupported()).WillRepeatedly(Return(false));
        EXPECT_CALL(*textureLoader, CreateHeightMap(_, _, _)).WillRepeatedly(Return(heightmap_.get()));

        auto terrainGo = scene->CreateGameObject("Terrain");
        terrainGo->SetLocalScale(vec3(100, 100, 100));
        LOG_DEBUG << "Add TerrainRendererComponent";

        EngineConf.renderer.type = GraphicsApi::RendererType::SIMPLE;
        auto& tc                 = terrainGo->AddComponent<Components::TerrainRendererComponent>();

        EXPECT_CALL(*textureLoader, GetGraphicsApi()).WillRepeatedly(ReturnRef(*graphicsApi));
        LOG_DEBUG << "Add createHeightMap";
        tc.createHeightMap();
        LOG_DEBUG << "AddGameObject";
        scene->AddGameObject(std::move(terrainGo));
        scene->ProcessEvents();
        LOG_DEBUG << "Return";

        meshTerrainComponent = tc.GetMeshTerrain();
        EXPECT_NE(meshTerrainComponent, nullptr);

        model = &meshTerrainComponent->GetModel();
    }
    void TearDown() override
    {
    }

    vec3 worldTerrainScale{100.f, 100.f, 100.f};
    ModelWrapper* model;
    std::unique_ptr<HeightMap> heightmap_;
    std::unique_ptr<TerrainMeshUpdater> sut_;
    Components::TerrainMeshRendererComponent* meshTerrainComponent;
};

TEST_F(TerrainMeshUpdaterTest, SimpleHeightMapUpdateCheck)
{
    EXPECT_NO_THROW(sut_->create());
    EXPECT_NE(model->Get(), nullptr);

    const auto& meshes = model->Get()->GetMeshes();
    ASSERT_GE(meshes.size(), 2u);

    const uint32 partsCount = EngineConf.renderer.terrain.meshPartsCount;
    auto partialSize = heightmap_->GetImage().width / partsCount;
    auto checkHeightMapVertex = [&](const vec2ui& point, uint32 meshIndex, float expectedHeight)
    {
        heightmap_->SetHeight(point, expectedHeight);
        meshTerrainComponent->HeightMapChanged();

        auto& meshData = meshes[meshIndex].GetCMeshDataRef();
        EXPECT_NEAR(meshData.positions_[1], expectedHeight, 0.01f)
            << "Mesh " << meshIndex << ", vertex at (" << point.x << "," << point.y
            << ") should match height";
    };

    float newHeightBase = 10.f;
    uint32 expectedMeshIndex = 0;
     for (uint32 j = 0; j < partsCount; ++j)
    {
        for (uint32 i = 0; i < partsCount; ++i)
        {
            checkHeightMapVertex({i* partialSize, j * partialSize}, expectedMeshIndex++, newHeightBase);
            newHeightBase += newHeightBase;
        }
    }
}
