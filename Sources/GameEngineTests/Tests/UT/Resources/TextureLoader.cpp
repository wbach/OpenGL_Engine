#include <Resources/GpuResourceLoader.h>
#include <Resources/TextureLoader.h>
#include <Resources/Textures/Image.h>
#include <Resources/Textures/Texture.h>
#include "GameEngineTests/Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "../../Mocks/Api/GraphicsApiMock.h"
#include <gtest/gtest.h>
#include "Logger/Log.h"

namespace GameEngine
{
struct TextureLoaderTest : public ::testing::Test
{
    TextureLoaderTest()
    {
        gpuLoader_.reset(new GpuResourceLoaderMock());
        apiMock_.reset(new GraphicsApiMock());
    }

    virtual void SetUp() override
    {
        sut_.reset(new TextureLoader(apiMock_, textures_, gpuLoader_));
    }
    std::shared_ptr<GraphicsApiMock> apiMock_;
    std::vector<std::unique_ptr<Texture>> textures_;
    std::shared_ptr<GpuResourceLoaderMock> gpuLoader_;
    std::shared_ptr<TextureLoader> sut_;
};

TEST_F(TextureLoaderTest, LoadTexture)
{
    //const std::string& file, SImage& image, bool applySizeLimit = true, TextureFlip::Type flip_mode = TextureFlip::Type::NONE
    Image image;
    sut_->ReadFile("", image);

    EXPECT_TRUE(true);
    // Re
}
}
