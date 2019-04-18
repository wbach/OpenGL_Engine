#include "Scene/Scene.hpp"
#include <gtest/gtest.h>
#include "GameEngineTests/Tests/Mocks/Api/WindowApiMock.hpp"
#include "GameEngineTests/Tests/Mocks/Renderers/GuiTextFactoryMock.h"

namespace GameEngine
{
namespace UT
{
struct SceneTest : public Scene
{
    SceneTest(const std::string& name)
        : Scene(name)
    {
        MakeGuiManager([](auto&) {});
        guiTextFactoryMock_ = new GuiTextFactoryMock();
        guiTextFactory_ = std::unique_ptr<GuiTextFactoryMock>(guiTextFactoryMock_);
    }

    void ReadGuiFile(const std::string& file)
    {
        Scene::ReadGuiFile(file);
    }

    GuiTextFactoryMock* guiTextFactoryMock_;
};
struct SceneShould : public ::testing::Test
{
    SceneShould()
        : sut_("UT_Scene")
    {
    }

    SceneTest sut_;
};

TEST_F(SceneShould, GuiFileReader)
{
    sut_.ReadGuiFile("../Sources/TestGame/gui.xml");
}

}  // namespace UT
}  // namespace GameEngine
