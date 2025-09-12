#include <GameEngine/Resources/Models/WBLoader/Assimp/AssimpLoader.h>
#include <Logger/Log.h>
#include <gtest/gtest.h>
#include "GameEngine/Engine/Configuration.h"
#include "Tests/Mocks/Api/GraphicsApiMock.h"
#include "Tests/Mocks/Resources/TextureLoaderMock.h"

using namespace ::testing;

namespace GameEngine
{
namespace UT
{
struct AssimpLoaderShould : public ::testing::Test
{
    AssimpLoaderShould()
    {
        EngineConf.useBinaryLoading = false;
        CLogger::Instance().EnableLogs(LoggingLvl::ErrorWarningInfoDebug);
        CLogger::Instance().UseAsyncLogging(false);
    }

    void SetUp() override
    {
        EXPECT_CALL(textureLoaderMock_, GetGraphicsApi()).WillRepeatedly(ReturnRef(graphicsApiMock_));

        sut_ = std::make_unique<WBLoader::AssimpLoader>(textureLoaderMock_);
    }

    void PrintJoints(const GameEngine::Animation::Joint& joint, const std::string& of = "")
    {
        /* LOG TO FIX*/  LOG_ERROR << (of + joint.name + " (size : " + std::to_string(joint.size) + ")");

        for (const auto& child : joint.children)
        {
            PrintJoints(child, of + "--");
        }
    }

    void PrintJointsWithMatrix(const GameEngine::Animation::Joint& joint, const std::string& of = "")
    {
        /* LOG TO FIX*/  LOG_ERROR << (of + joint.name);
        /* LOG TO FIX*/  LOG_ERROR << (of + std::to_string(joint.id));
        /* LOG TO FIX*/  LOG_ERROR << (of + std::to_string(joint.transform));

        for (const auto& child : joint.children)
        {
            PrintJointsWithMatrix(child, of + "--");
        }
    }

    GraphicsApi::GraphicsApiMock graphicsApiMock_;
    TextureLoaderMock textureLoaderMock_;
    std::unique_ptr<WBLoader::AssimpLoader> sut_;
};
TEST_F(AssimpLoaderShould, DISABLED_ReadSimpleCube)
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

// Disabled because of data needed
TEST_F(AssimpLoaderShould, DISABLED_ReadGarenAnimations)
{
    EXPECT_CALL(textureLoaderMock_, LoadTexture(_, _)).WillOnce(Return(nullptr));

    // std::string file{"Meshes/DaeAnimationExample/Character.fbx"};
    //std::string file{"Meshes/Avatar/aangMakeHumanRig.Walkblend.fbx"};
    //std::string file{"Meshes/Garen/garen_idle_b.fbx"};
    std::string file{ "Meshes/DaeAnimationExample/CharacterRunning.dae" };
    ASSERT_TRUE(Utils::CheckFileExist("../Data/" + file));
    sut_->Parse(file);
    auto model = sut_->Create();
    auto data  = model->GetMeshes().front().GetCMeshDataRef();
    // EXPECT_EQ( model->skeleton_.children.size(), 1);

    /* LOG TO FIX*/  LOG_ERROR << ("Print skeleton");
    //PrintJointsWithMatrix(model->skeleton_);
    /* LOG TO FIX*/  LOG_ERROR << ("end print skeleton");
}
}  // namespace UT
}  // namespace GameEngine


