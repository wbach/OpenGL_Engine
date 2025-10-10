#include <GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h>
#include <Logger/Log.h>
#include <gtest/gtest.h>
#include "Tests/Mocks/Api/GraphicsApiMock.h"
#include <Utils/GLM/GLMUtils.h>

using namespace ::testing;

namespace GameEngine
{
struct TerrainToolsShould : public ::testing::Test
{
    TerrainToolsShould()
    {
    }

    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
    void init(const vec3& scale, const Utils::Image& image )
    {
        sut_ = std::make_unique<TerrainHeightTools>(scale, image);
    }

    std::unique_ptr<TerrainHeightTools> sut_;
};

TEST_F(TerrainToolsShould, vauleInRangeInZeroPos)
{
    vec3 terrainScale{ 1, 1, 1 };
    Utils::Image image;
    image.width = 32;
    image.height = 32;
    image.setChannels(1);
    image.allocateImage<float>();

    for(uint32 y = 0; y < image.height; ++y)
    {
        for(uint32 x = 0; x < image.width; ++x)
        {
            image.setPixel({x, y}, Color(0, 0, 0, 0));
        }
    }

    init(terrainScale, image);

    auto normal = sut_->GetNormal(0, 0);
    LOG_DEBUG << normal;

    EXPECT_FLOAT_EQ(normal.x, 0.f);
    EXPECT_FLOAT_EQ(normal.y, 1.f);
    EXPECT_FLOAT_EQ(normal.z, 0.f);

    auto transformMatrix = Utils::CreateTransformationMatrix(vec3(0), Quaternion(), vec3(1, 2, 1));

    auto transformedNormal = glm::normalize(vec3(transformMatrix * vec4(sut_->GetNormal(0, 0), 0.0f)));
    LOG_DEBUG << transformedNormal;

    EXPECT_FLOAT_EQ(transformedNormal.x, 0.f);
    EXPECT_FLOAT_EQ(transformedNormal.y, 1.f);
    EXPECT_FLOAT_EQ(transformedNormal.z, 0.f);
}

}  // namespace GameEngine
