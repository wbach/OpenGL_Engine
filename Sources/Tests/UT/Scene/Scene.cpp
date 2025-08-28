#include "GameEngine/Scene/Scene.hpp"
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <Utils/GLM/GLMUtils.h>
#include <gtest/gtest.h>
#include "GameEngine/Resources/ResourceManager.h"
#include "Tests/Mocks/Api/GraphicsApiMock.h"
#include "Tests/Mocks/Api/WindowApiMock.hpp"
#include "Tests/Mocks/Renderers/GuiTextFactoryMock.h"

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
    }

    ~SceneTest()
    {
        DEBUG_LOG("");
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

    GraphicsApi::WindowApiMock windowApiMock_;
    SceneTest sut_;
};

}  // namespace UT
}  // namespace GameEngine
