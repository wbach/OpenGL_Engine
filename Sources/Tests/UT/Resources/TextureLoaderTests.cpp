#include <GameEngine/Resources/GpuResourceLoader.h>
#include <GameEngine/Resources/TextureLoader.h>
#include <GameEngine/Resources/Textures/Texture.h>
#include <Utils/Time/Timer.h>
#include <gtest/gtest.h>
#include <filesystem>
#include "../../Mocks/Api/GraphicsApiMock.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "Logger/Log.h"

namespace GameEngine
{
struct TextureLoaderTest : public ::testing::Test
{
    TextureLoaderTest()
    {
        const std::string configFile = "./Conf.xml";
        GameEngine::ReadFromFile(configFile);
    }

    virtual void SetUp() override
    {
        sut_.reset(new TextureLoader(apiMock_, gpuLoader_));
    }
    GraphicsApi::GraphicsApiMock apiMock_;
    std::unordered_map<std::string, ResourceInfo<Texture>> textures_;
    GpuResourceLoaderMock gpuLoader_;
    std::unique_ptr<TextureLoader> sut_;
};

// Disabled because of data needed
TEST_F(TextureLoaderTest, DISABLED_FreeImageForValgrind)
{
    Utils::Timer timer;
    EngineConf.renderer.textures.maxSize = vec2ui(2048, 2048);
    File textureFile{"Textures/textureNotFound.png"};
    EXPECT_TRUE(std::filesystem::exists(textureFile.GetAbsolutePath()));
    GameEngine::ReadFile(textureFile, TextureParameters());
    /* LOG TO FIX*/  LOG_ERROR << ("Time : " + std::to_string(timer.GetTimeNanoseconds()));
}
// Disabled because of data needed
TEST_F(TextureLoaderTest, DISABLED_FreeImageResizedForValgrind)
{
    Utils::Timer timer;
    EngineConf.renderer.textures.maxSize = vec2ui(320, 240);
    File textureFile{"Textures/textureNotFound.png"};
    EXPECT_TRUE(std::filesystem::exists(textureFile.GetAbsolutePath()));
    GameEngine::ReadFile(textureFile, TextureParameters());
    /* LOG TO FIX*/  LOG_ERROR << ("Time : " + std::to_string(timer.GetTimeNanoseconds()));
}
}  // namespace GameEngine
