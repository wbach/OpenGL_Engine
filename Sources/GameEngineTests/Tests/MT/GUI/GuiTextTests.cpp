#include <gtest/gtest.h>
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngineTests/Tests/UT/Components/BaseComponent.h"
#include "OpenGLApi/SDL2/SDLOpenGL.h"
#include "Utils/GLM/GLMUtils.h"

namespace GameEngine
{
namespace MT
{
const vec2ui windowSize{640, 480};
const std::string fontStr = "../Data/GUI/Ubuntu-M.ttf";

struct GuiTextElementShould : public BaseComponentTestSchould
{
    GuiTextElementShould()
    {
        sdlApi_.Init();
        sut_ = std::make_unique<GuiTextElement>(
            std::bind(&GuiTextElementShould::UpdateTextureFunction, this, std::placeholders::_1), sdlApi_, windowSize,
            fontStr);
    }
    void SetUp()
    {
    }
    void UpdateTextureFunction(GuiTextElement&)
    {
    }
    OpenGLApi::SdlOpenGlApi sdlApi_;
    std::unique_ptr<GuiTextElement> sut_;
};
TEST_F(GuiTextElementShould, MCT_SDL_UpdateText)
{
    for (int i = 0; i < 1000; ++i)
        sut_->SetText("New text " + std::to_string(i));

    ASSERT_TRUE(sut_->GetSurface());
    auto& surface = *sut_->GetSurface();
    DEBUG_LOG("Surface size : " + std::to_string(surface.size));
    EXPECT_TRUE(surface.size.x > 0);
    EXPECT_TRUE(surface.size.y > 0);
}
}  // namespace MT
}  // namespace GameEngine