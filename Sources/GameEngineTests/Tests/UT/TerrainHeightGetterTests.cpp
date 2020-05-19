#include <GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <Logger/Log.h>
#include <gtest/gtest.h>
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"

using namespace ::testing;

namespace GameEngine
{
struct TerrainHeightGetterShould : public ::testing::Test
{
    TerrainHeightGetterShould()
    {
        uint32 heightMapSize = 100;
        Image image;
        image.width  = heightMapSize;
        image.height = heightMapSize;
        image.floatData.resize(heightMapSize * heightMapSize);
        for (size_t i = 0; i < heightMapSize * heightMapSize; ++i)
        {
            image.floatData[i] = 0;
        }

        heightMap_ = std::make_unique<HeightMap>(graphicsApiMock_, "file", image);

        heightMap_->SetHeight(vec2ui(0, 0), 10.f);
        heightMap_->SetHeight(vec2ui(0, heightMapSize - 1), 10.f);
        heightMap_->SetHeight(vec2ui(heightMapSize - 1, 0), 10.f);
        heightMap_->SetHeight(vec2ui(heightMapSize - 1, heightMapSize - 1), 10.f);
    }
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }

    void SetupSut(const vec2& terrainPosition)
    {
        sut_ = std::make_unique<TerrainHeightGetter>(terrainConfig_, *heightMap_, terrainPosition);
    }

    void ExpectOutOfRange(const vec2& terrainPosition)
    {
        SetupSut(terrainPosition);

        auto scale = terrainConfig_.GetScale() / 2.f + 0.01f;

        {
            auto h = sut_->GetHeightofTerrain(scale.x + terrainPosition.x, terrainPosition.y);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, scale.z + terrainPosition.y);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(scale.x + terrainPosition.x, scale.z + terrainPosition.y);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(-scale.x + terrainPosition.x, terrainPosition.y);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, -scale.z + terrainPosition.y);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(-scale.x + terrainPosition.x, -scale.z + terrainPosition.y);
            EXPECT_FALSE(h.has_value());
        }
    }
    void ExpectInside(const vec2& terrainPosition)
    {
        SetupSut(terrainPosition);

        auto scale = terrainConfig_.GetScale() / 2.f - 0.01f;

        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, terrainPosition.y);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(scale.x + terrainPosition.x, terrainPosition.y);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, scale.z + terrainPosition.y);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(scale.x + terrainPosition.x, scale.z + terrainPosition.y);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(-scale.x + terrainPosition.x, terrainPosition.y);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, -scale.z + terrainPosition.y);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(-scale.x + terrainPosition.x, -scale.z + terrainPosition.y);
            EXPECT_TRUE(h.has_value());
        }
    }
    GraphicsApi::GraphicsApiMock graphicsApiMock_;
    TerrainConfiguration terrainConfig_;
    std::unique_ptr<HeightMap> heightMap_;
    std::unique_ptr<TerrainHeightGetter> sut_;
};

TEST_F(TerrainHeightGetterShould, vauleInRangeInZeroPos)
{
    ExpectInside(vec2(0));
}
TEST_F(TerrainHeightGetterShould, vauleInRangeInOtherPos)
{
    ExpectInside(vec2(500, 300));
}
TEST_F(TerrainHeightGetterShould, vauleInRangeInOtherPos2)
{
    ExpectInside(vec2(-500, 300));
}
TEST_F(TerrainHeightGetterShould, vauleInRangeInOtherPos3)
{
    ExpectInside(vec2(-3500, 2221300));
}
TEST_F(TerrainHeightGetterShould, vauleInOutOfRangeInZeroPos)
{
    ExpectOutOfRange(vec2(0));
}
TEST_F(TerrainHeightGetterShould, vauleInOutOfRangeInOtherPos)
{
    ExpectOutOfRange(vec2(100, 20));
}
TEST_F(TerrainHeightGetterShould, vauleInOutOfRangeInOtherPos2)
{
    ExpectOutOfRange(vec2(100, -20));
}
TEST_F(TerrainHeightGetterShould, vauleInOutOfRangeInOtherPos3)
{
    ExpectOutOfRange(vec2(10000, -23000));
}
}  // namespace GameEngine
