#include <GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <Logger/Log.h>
#include <gtest/gtest.h>

#include "Tests/Mocks/Api/GraphicsApiMock.h"

using namespace ::testing;

namespace GameEngine
{
namespace
{
const float HEIGHT_MAP_VALUE = 10.f;
const uint32 HEIGHT_MAP_SIZE = 101;

struct TerrainTestParams
{
    std::string name;
    glm::vec2 offset;
};
}  // namespace

// ---------------------------
// Wspólny helper z oryginalnymi metodami
// ---------------------------
struct TerrainHeightGetterHelper
{
    GraphicsApi::GraphicsApiMock graphicsApiMock_;
    vec3 terrainScale_{100, 20, 100};
    std::unique_ptr<HeightMap> heightMap_;
    std::unique_ptr<TerrainHeightGetter> sut_;

    void InitHeightMap()
    {
        Utils::Image image;
        image.width  = HEIGHT_MAP_SIZE;
        image.height = HEIGHT_MAP_SIZE;
        image.setChannels(1);
        image.allocateImage<float>();
        heightMap_ = std::make_unique<HeightMap>(graphicsApiMock_, GameEngine::TextureParameters(), "file", std::move(image));
    }

    void SetupSut(const vec3& terrainPosition)
    {
        sut_ = std::make_unique<TerrainHeightGetter>(terrainScale_, *heightMap_, terrainPosition);
    }

    void ExpectOutOfRange(const vec3& terrainPosition)
    {
        SetupSut(terrainPosition);
        auto scale = terrainScale_ / 2.f + 0.01f;

        {
            auto h = sut_->GetHeightofTerrain(scale.x + terrainPosition.x, terrainPosition.z);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, scale.z + terrainPosition.z);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(scale.x + terrainPosition.x, scale.z + terrainPosition.z);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(-scale.x + terrainPosition.x, terrainPosition.z);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, -scale.z + terrainPosition.z);
            EXPECT_FALSE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(-scale.x + terrainPosition.x, -scale.z + terrainPosition.z);
            EXPECT_FALSE(h.has_value());
        }
    }

    void ExpectInside(const vec3& terrainPosition)
    {
        SetupSut(terrainPosition);
        auto scale = terrainScale_ / 2.f - 0.01f;

        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, terrainPosition.z);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(scale.x + terrainPosition.x, terrainPosition.z);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, scale.z + terrainPosition.z);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(scale.x + terrainPosition.x, scale.z + terrainPosition.z);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(-scale.x + terrainPosition.x, terrainPosition.z);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(terrainPosition.x, -scale.z + terrainPosition.z);
            EXPECT_TRUE(h.has_value());
        }
        {
            auto h = sut_->GetHeightofTerrain(-scale.x + terrainPosition.x, -scale.z + terrainPosition.z);
            EXPECT_TRUE(h.has_value());
        }
    }
};

// ---------------------------
// TEST_F (zwykłe testy) korzystają z helpera
// ---------------------------
struct TerrainHeightGetterShould : public ::testing::Test, public TerrainHeightGetterHelper
{
    void SetUp() override
    {
        InitHeightMap();
    }
};

TEST_F(TerrainHeightGetterShould, vauleInRangeInZeroPos)
{
    ExpectInside(vec3(0));
}
TEST_F(TerrainHeightGetterShould, vauleInRangeInOtherPos)
{
    ExpectInside(vec3(500, 0, 300));
}
TEST_F(TerrainHeightGetterShould, vauleInRangeInOtherPos2)
{
    ExpectInside(vec3(-500, 0, 300));
}
TEST_F(TerrainHeightGetterShould, vauleInRangeInOtherPos3)
{
    ExpectInside(vec3(-3500, 0, 2221300));
}
TEST_F(TerrainHeightGetterShould, vauleInOutOfRangeInZeroPos)
{
    ExpectOutOfRange(vec3(0));
}
TEST_F(TerrainHeightGetterShould, vauleInOutOfRangeInOtherPos)
{
    ExpectOutOfRange(vec3(100, 0, 20));
}
TEST_F(TerrainHeightGetterShould, vauleInOutOfRangeInOtherPos2)
{
    ExpectOutOfRange(vec3(100, 0, -20));
}
TEST_F(TerrainHeightGetterShould, vauleInOutOfRangeInOtherPos3)
{
    ExpectOutOfRange(vec3(10000, 0, -23000));
}

// ---------------------------
// TEST_P (parametryzowane) korzystają z helpera
// ---------------------------
struct TerrainHeightGetterParamTest : public ::testing::TestWithParam<TerrainTestParams>, public TerrainHeightGetterHelper
{
    void SetUp() override
    {
        InitHeightMap();
    }
};

TEST_P(TerrainHeightGetterParamTest, checkHeightIsProperlyGet)
{
    vec3 terrainPosition{10, 15, 5};
    SetupSut(terrainPosition);

    const auto& param = GetParam();

    // Wyliczamy pixel na heightMap z uwzględnieniem offsetu (-1..+1)
    uint32_t mapX = static_cast<uint32_t>(std::round((param.offset.x + 1.0) / 2.0 * (HEIGHT_MAP_SIZE - 1)));
    uint32_t mapY = static_cast<uint32_t>(std::round((param.offset.y + 1.0) / 2.0 * (HEIGHT_MAP_SIZE - 1)));

    // Ustawiamy pixel na mapie wysokości
    heightMap_->SetHeight(vec2ui(mapX, mapY), HEIGHT_MAP_VALUE);

    // Obliczamy pozycję testową w świecie
    double testX = terrainPosition.x + param.offset.x * (terrainScale_.x / 2.0);
    double testZ = terrainPosition.z + param.offset.y * (terrainScale_.z / 2.0);

    auto h = sut_->GetHeightofTerrain(testX, testZ);
    EXPECT_TRUE(h.has_value()) << "Height not found for " << param.name;
    EXPECT_NEAR(h.value(), HEIGHT_MAP_VALUE * terrainScale_.y + terrainPosition.y, 0.01f) << "at position: " << param.name;
}

// ---------------------------
// Parametryzacja
// ---------------------------
INSTANTIATE_TEST_SUITE_P(TerrainHeightPositions, TerrainHeightGetterParamTest,
                         ::testing::Values(TerrainTestParams{"center", {0.0, 0.0}}, TerrainTestParams{"top_left", {-1.0, +1.0}},
                                           TerrainTestParams{"top_right", {+1.0, +1.0}},
                                           TerrainTestParams{"bottom_left", {-1.0, -1.0}},
                                           TerrainTestParams{"bottom_right", {+1.0, -1.0}}),
                         [](const testing::TestParamInfo<TerrainTestParams>& info) { return info.param.name; });

}  // namespace GameEngine
