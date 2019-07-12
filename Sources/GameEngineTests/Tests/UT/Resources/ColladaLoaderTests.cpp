#include <gtest/gtest.h>
#include "Engine/Configuration.h"
#include "GameEngine/Resources/Models/WBLoader/Collada/Collada.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "GameEngineTests/Tests/Mocks/Resources/TextureLoaderMock.h"

using namespace ::testing;

namespace GameEngine
{
namespace WBLoader
{
struct ColladaLoaderShould : public ::testing::Test
{
    ColladaLoaderShould()
    {
        EngineConf.useBinaryLoading = false;
    }
    void SetUp()
    {
        EXPECT_CALL(textureLoaderMock_, GetGraphicsApi()).WillRepeatedly(ReturnRef(apiMock_));
        sut_.reset(new ColladaDae(textureLoaderMock_));
    }
    TextureLoaderMock textureLoaderMock_;
    GraphicsApi::GraphicsApiMock apiMock_;
    std::unique_ptr<ColladaDae> sut_;
};

TEST_F(ColladaLoaderShould, GetFloatsFromString)
{
    std::vector<float> ref({1.f,        1.f,         -1.f, 1.f,         -1.f, -1.f,       -1.f, -0.9999998f,
                            -1.f,       -0.9999997f, 1.f,  -1.f,        1.f,  0.9999995f, 1.f,  0.9999994f,
                            -1.000001f, 1.f,         -1.f, -0.9999997f, 1.f,  -1.f,       1.f,  1.f});

    std::string str =
        "1 1 -1 1 -1 -1 -1 -0.9999998 -1 -0.9999997 1 -1 1 0.9999995 1 0.9999994 -1.000001 1 -1 -0.9999997 1 -1 1 1";

    auto result = GameEngine::Collada::GetFloatsFromString(str);

    ASSERT_EQ(result.size(), ref.size());

    for (uint32 x = 0; x < ref.size(); ++x)
    {
        EXPECT_EQ(ref[x], result[x]);
    }
}
TEST_F(ColladaLoaderShould, ReadSimpleCube)
{
    std::string cubeFile{"Meshes/box.dae"};
    ASSERT_TRUE(Utils::CheckFileExist("../Data/" + cubeFile));

    sut_->Parse(cubeFile);
    auto model = sut_->Create();
    auto data  = model->GetMeshes().front().GetCMeshDataRef();

    EXPECT_EQ(data.positions_.size(), 24);
    EXPECT_EQ(data.indices_.size(), 36);

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
}  // namespace WBLoader
}  // namespace GameEngine
