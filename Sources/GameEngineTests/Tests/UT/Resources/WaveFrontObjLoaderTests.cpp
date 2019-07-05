#include <GameEngine/Resources/Models/WBLoader/WaveFront/WaveFrontObj.h>
#include <gtest/gtest.h>
#include "GameEngine/Engine/Configuration.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Resources/TextureLoaderMock.h"

using namespace ::testing;

namespace GameEngine
{
namespace UT
{
struct WaveFrontObjLoaderShould : public ::testing::Test
{
    WaveFrontObjLoaderShould()
    {
        EngineConf.useBinaryLoading = false;
    }

    void SetUp() override
    {
        EXPECT_CALL(textureLoaderMock_, GetGraphicsApi()).WillRepeatedly(ReturnRef(graphicsApiMock_));

        sut_ = std::make_unique<WBLoader::WaveFrontObjLoader>(textureLoaderMock_);
    }

    GraphicsApi::GraphicsApiMock graphicsApiMock_;
    TextureLoaderMock textureLoaderMock_;
    std::unique_ptr<WBLoader::WaveFrontObjLoader> sut_;
};
TEST_F(WaveFrontObjLoaderShould, ReadSimpleCube)
{
    std::string cubeFile{"Meshes/box.obj"};
    ASSERT_TRUE(Utils::CheckFileExist("../Data/" + cubeFile));
    sut_->Parse(cubeFile);
    auto model = sut_->Create();
    auto data  = model->GetMeshes().front().GetCMeshDataRef();

    EXPECT_EQ(data.positions_.size(), 24);
    EXPECT_EQ(data.indices_.size(), 8);

    std::cout << "positions_ size : " << data.positions_.size() << std::endl;
    std::cout << "indices_ size : " << data.indices_.size() << std::endl;

    for (auto i : data.indices_)
    {
        std::cout << i << " ";
    }

    std::cout << std::endl;

    for (const auto& i : data.positions_)
    {
        std::cout << i << " ";
    }

    std::cout << std::endl;
}
}  // namespace UT
}  // namespace GameEngine
