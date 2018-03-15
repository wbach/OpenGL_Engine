#include <Resources/OpenGLLoader.h>
#include <Resources/TextureLoader.h>
#include <Resources/Textures/Image.h>
#include <Resources/Textures/Texture.h>
#include <gtest/gtest.h>
#include "Logger/Log.h"
struct TextureLoaderTest : public CTextureLoader, public ::testing::Test
{
    TextureLoaderTest() : CTextureLoader(textures, openGLLoader)
    {
    }

    virtual void SetUp() override
    {
        //CLogger::Instance().EnableLogs();
    }

    std::vector<std::unique_ptr<CTexture>> textures;
    COpenGLLoader openGLLoader;
};

TEST_F(TextureLoaderTest, LoadTexture)
{
    //const std::string& file, SImage& image, bool applySizeLimit = true, TextureFlip::Type flip_mode = TextureFlip::Type::NONE
    SImage image;
    ReadFile("", image);

    EXPECT_TRUE(true);
    // Re
}
