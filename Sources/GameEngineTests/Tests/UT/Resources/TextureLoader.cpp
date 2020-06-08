#include <GameEngine/Resources/GpuResourceLoader.h>
#include <GameEngine/Resources/TextureLoader.h>
#include <GameEngine/Resources/Textures/Texture.h>
#include <Utils/Time/Timer.h>
#include <gtest/gtest.h>
#include <filesystem>
#include "../../Mocks/Api/GraphicsApiMock.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngineTests/Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "Logger/Log.h"

namespace GameEngine
{
struct TextureLoaderTest : public ::testing::Test
{
    TextureLoaderTest()
    {
        const std::string configFile = "./Conf.xml";
        GameEngine::ReadFromFile(configFile);
        CLogger::Instance().EnableLogs();
        CLogger::Instance().ImmeditalyLog();
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

TEST_F(TextureLoaderTest, FreeImageForValgrind)
{
    Utils::Timer timer;
    EngineConf.renderer.textures.maxSize = vec2ui(2048, 2048);
    File textureFile{"Textures/textureNotFound.png"};
    EXPECT_TRUE(std::filesystem::exists(textureFile.GetAbsoultePath()));
    GameEngine::ReadFile(textureFile, TextureParameters());
    DEBUG_LOG("Time : " + std::to_string(timer.GetTimeNanoseconds()));
}
TEST_F(TextureLoaderTest, FreeImageResizedForValgrind)
{
    Utils::Timer timer;
    EngineConf.renderer.textures.maxSize = vec2ui(320, 240);
    File textureFile{"Textures/textureNotFound.png"};
    EXPECT_TRUE(std::filesystem::exists(textureFile.GetAbsoultePath()));
    GameEngine::ReadFile(textureFile, TextureParameters());
    DEBUG_LOG("Time : " + std::to_string(timer.GetTimeNanoseconds()));
}
}  // namespace GameEngine
