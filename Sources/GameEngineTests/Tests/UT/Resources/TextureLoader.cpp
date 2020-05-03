#include <Resources/GpuResourceLoader.h>
#include <Resources/TextureLoader.h>
#include <Resources/Textures/Image.h>
#include <Resources/Textures/Texture.h>
#include <gtest/gtest.h>
#include "../../Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "Logger/Log.h"

namespace GameEngine
{
struct TextureLoaderTest : public ::testing::Test
{
    TextureLoaderTest()
    {
    }

    virtual void SetUp() override
    {
        sut_.reset(new TextureLoader(apiMock_, gpuLoader_, textures_));
    }
    GraphicsApi::GraphicsApiMock apiMock_;
    std::unordered_map<std::string, ResourceInfo<Texture>> textures_;
    GpuResourceLoaderMock gpuLoader_;
    std::unique_ptr<TextureLoader> sut_;
};

TEST_F(TextureLoaderTest, DummyTest)
{
    EXPECT_TRUE(true);
}
}  // namespace GameEngine
