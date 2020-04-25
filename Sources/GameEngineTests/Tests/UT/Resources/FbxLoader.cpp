#include <GameEngine/Resources/Models/WBLoader/Fbx/FbxLoader.h>
#include <gtest/gtest.h>
#include "GameEngine/Engine/Configuration.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Resources/TextureLoaderMock.h"

using namespace ::testing;

namespace GameEngine
{
namespace UT
{
struct FbxLoaderShould : public ::testing::Test
{
    FbxLoaderShould()
    {
        EngineConf.useBinaryLoading = false;
    }

    void SetUp() override
    {
        EXPECT_CALL(textureLoaderMock_, GetGraphicsApi()).WillRepeatedly(ReturnRef(graphicsApiMock_));

        sut_ = std::make_unique<WBLoader::FbxLoader>(textureLoaderMock_);
    }

    GraphicsApi::GraphicsApiMock graphicsApiMock_;
    TextureLoaderMock textureLoaderMock_;
    std::unique_ptr<WBLoader::FbxLoader> sut_;
};
TEST_F(FbxLoaderShould, ReadSimpleCube)
{
    std::string cubeFile{"Meshes/box.fbx"};
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

void PrintJoints(const GameEngine::Animation::Joint& joint, const std::string& of = "")
{
    std::cout << of << joint.name << std::endl;

    for (const auto& child : joint.children)
    {
       PrintJoints(child, of + "--");
    }
}

void PrintJointsWithMatrix(const GameEngine::Animation::Joint& joint, const std::string& of = "")
{
    std::cout << of << joint.name << std::endl;
    std::cout << of << std::to_string(joint.transform) << std::endl;

    for (const auto& child : joint.children)
    {
       PrintJointsWithMatrix(child, of + "--");
    }
}

TEST_F(FbxLoaderShould, ReadGarenAnimations)
{
    EXPECT_CALL(textureLoaderMock_, LoadTexture(_, _)).WillOnce(Return(nullptr));
    std::string file{"Meshes/Garen/garen_idle_b.fbx"};
    ASSERT_TRUE(Utils::CheckFileExist("../Data/" + file));
    sut_->Parse(file);
    auto model = sut_->Create();
    auto data  = model->GetMeshes().front().GetCMeshDataRef();
    //EXPECT_EQ( model->skeleton_.children.size(), 1);

    PrintJoints(model->skeleton_);
}
}  // namespace UT
}  // namespace GameEngine
