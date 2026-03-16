#include <GameEngine/Resources/GpuResourceLoader.h>
#include <GameEngine/Resources/TextureLoader.h>
#include <GameEngine/Resources/Textures/Texture.h>
#include <Utils/Time/Stopwatch.h>
#include <gtest/gtest.h>

#include <filesystem>

#include "../../Mocks/Api/GraphicsApiMock.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "Logger/Log.h"
#include "Tests/Mocks/Resources/GpuResourceLoaderMock.h"

namespace GameEngine
{
struct TextureLoaderTest : public ::testing::Test
{
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
    Utils::Stopwatch timer;
    EngineConf.renderer.textures.maxSize = vec2ui(2048, 2048);
    File textureFile{"Textures/textureNotFound.png"};
    EXPECT_TRUE(std::filesystem::exists(textureFile.GetAbsolutePath()));
    GameEngine::ReadFile(textureFile, TextureParameters());
    LOG_DEBUG << "Time : " << timer.GetTimeNanoseconds();
}
// Disabled because of data needed
TEST_F(TextureLoaderTest, DISABLED_FreeImageResizedForValgrind)
{
    Utils::Stopwatch timer;
    EngineConf.renderer.textures.maxSize = vec2ui(320, 240);
    File textureFile{"Textures/textureNotFound.png"};
    EXPECT_TRUE(std::filesystem::exists(textureFile.GetAbsolutePath()));
    GameEngine::ReadFile(textureFile, TextureParameters());
    LOG_DEBUG << "Time : " << timer.GetTimeNanoseconds();
}
}  // namespace GameEngine
