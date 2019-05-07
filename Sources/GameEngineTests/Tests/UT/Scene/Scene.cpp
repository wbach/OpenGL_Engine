#include "Scene/Scene.hpp"
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <Utils/GLM/GLMUtils.h>
#include <gtest/gtest.h>
#include "GameEngineTests/Tests/Mocks/Api/WindowApiMock.hpp"
#include "GameEngineTests/Tests/Mocks/Renderers/GuiTextFactoryMock.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Api/WindowApiMock.hpp"
#include "GameEngine/Resources/ResourceManager.h"

using namespace testing;

namespace GameEngine
{
namespace UT
{
const vec2ui WINDOW_SIZE{640, 480};
const std::string FONT = "../Data/GUI/Ubuntu-M.ttf";

struct SceneTest : public Scene
{
    SceneTest(const std::string& name)
        : Scene(name)
    {
        EXPECT_CALL(graphicsApiMock_, GetWindowApi()).WillRepeatedly(ReturnRef(windowApiMock_));
        CreateResourceManger(new ResourceManager(graphicsApiMock_));
        MakeGuiManager([](auto&) {});
        guiTextFactoryMock_ = new GuiTextFactoryMock();
        guiTextFactory_     = std::unique_ptr<GuiTextFactoryMock>(guiTextFactoryMock_);
    }

    ~SceneTest()
    {
        DEBUG_LOG("");
    }

    void ReadGuiFile(const std::string& file)
    {
        Scene::ReadGuiFile(file);
    }

    GuiTextElement* CreateGuiText(const std::string& label, const std::string& font, const std::string& str,
                                  uint32 size, uint32 outline)
    {
        return Scene::CreateGuiText(label, font, str, size, outline);
    }

    GuiTextElement* GuiText(const std::string& label)
    {
        return Scene::GuiText(label);
    }

    GraphicsApi::WindowApiMock windowApiMock_;
    GraphicsApi::GraphicsApiMock graphicsApiMock_;
    GuiTextFactoryMock* guiTextFactoryMock_;
};
struct SceneShould : public ::testing::Test
{
    SceneShould()
        : sut_("UT_Scene")
    {
    }

    ~SceneShould()
    {
        DEBUG_LOG("");
    }

    void CrateElements()
    {
        EXPECT_CALL(*sut_.guiTextFactoryMock_, CreateImpl(_, _, _, _))
            .WillRepeatedly(Return(new GuiTextElement([](auto&) {}, windowApiMock_, WINDOW_SIZE, FONT)));
        EXPECT_CALL(windowApiMock_, OpenFont(_, _)).WillRepeatedly(Return(std::optional<uint32>()));
        sut_.CreateGuiText("rendererFps", FONT, "rendererFps ", 10, 0);
    }

    GraphicsApi::WindowApiMock windowApiMock_;
    SceneTest sut_;
};

TEST_F(SceneShould, GuiFileReader)
{
    CrateElements();
    auto renderText = sut_.GuiText("rendererFps");
    auto mat        = renderText->GetMatrix();
    DEBUG_LOG(std::to_string(mat));
}

}  // namespace UT
}  // namespace GameEngine
