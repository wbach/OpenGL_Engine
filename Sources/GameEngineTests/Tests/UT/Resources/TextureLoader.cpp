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
        gpuLoader_.reset(new GpuResourceLoaderMock());
    }

    virtual void SetUp() override
    {
        sut_.reset(new TextureLoader(apiMock_, textures_, gpuLoader_));
    }
    GraphicsApi::GraphicsApiMock apiMock_;
    std::vector<std::unique_ptr<Texture>> textures_;
    std::shared_ptr<GpuResourceLoaderMock> gpuLoader_;
    std::shared_ptr<TextureLoader> sut_;
};

TEST_F(TextureLoaderTest, LoadTexture)
{
    auto image = sut_->ReadFile("");

    EXPECT_TRUE(true);
}
}  // namespace GameEngine
