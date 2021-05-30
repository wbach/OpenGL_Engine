#include <GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <GameEngine/Objects/GameObject.h>
#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>
#include <gtest/gtest.h>
#include <GameEngineTests/Tests/UT/Components/BaseComponent.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h>
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"

using namespace ::testing;

namespace GameEngine
{
//const float textureTiledSize = 1.f;

struct TerrainTransitionShould : public BaseComponentTestSchould
{
    TerrainTransitionShould()
        : go1("go1", componentController_, componentFactory_, 1)
        , go2("go2", componentController_, componentFactory_, 2)
    {
        CLogger::Instance().EnableLogs();
        CLogger::Instance().ImmeditalyLog();

        //auto& terrainComponent1 = go1.AddComponent<Components::TerrainRendererComponent>();
        //auto& terrainComponent2 = go2.AddComponent<Components::TerrainRendererComponent>();

        Utils::Image image1;
        Utils::Image image2;
        heightMap1_ = std::make_unique<HeightMap>(graphicsApiMock_, TextureParameters{}, File{}, image1);
        heightMap2_ = std::make_unique<HeightMap>(graphicsApiMock_, TextureParameters{}, File{}, image2);

        //terrainComponent1.setTexture(*heightMap1_, textureTiledSize, Components::TerrainTextureType::HeightMap);
        //terrainComponent2.setTexture(*heightMap2_, textureTiledSize, Components::TerrainTextureType::HeightMap);
    }

    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
    void init()
    {

    }

    GameObject go1;
    GameObject go2;
    std::unique_ptr<HeightMap> heightMap1_;
    std::unique_ptr<HeightMap> heightMap2_;
};

TEST_F(TerrainTransitionShould, vauleInRangeInZeroPos)
{

}

}  // namespace GameEngine
