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
        textureLoaderMock_.reset(new TextureLoaderMock());

        EXPECT_CALL(*textureLoaderMock_, GetGraphicsApi()).WillOnce(ReturnRef(apiMock_));
        sut_.reset(new ColladaDae(*textureLoaderMock_));
    }
    std::shared_ptr<TextureLoaderMock> textureLoaderMock_;
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
}  // namespace WBLoader
}  // namespace GameEngine
