#include <gtest/gtest.h>

#include "GameEngine/Renderers/GUI/Renderer.h"
#include "GameEngine/Renderers/GUI/Text/FontManager.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
#include "Tests/Mocks/Api/GraphicsApiMock.h"
#include "Tests/Mocks/Resources/ResourcesManagerMock.h"
#include "Tests/UT/Components/BaseComponent.h"
#include "Utils/GLM/GLMUtils.h"

namespace GameEngine
{
namespace MT
{
const std::string fontStr = "../Data/GUI/Ubuntu-M.ttf";

struct GuiTextElementShould : public BaseComponentTestSchould
{
    GuiTextElementShould()
        : guiRenderer_(graphicsApi_)
    {
        sut_ = std::make_unique<GUI::Text>(fontManager_, resourcesManager_, guiRenderer_, fontStr);
    }

    GraphicsApi::GraphicsApiMock graphicsApi_;
    GUI::Renderer guiRenderer_;
    ResourceManagerMock resourcesManager_;
    GUI::FontManager fontManager_;
    std::unique_ptr<GUI::Text> sut_;
};
}  // namespace MT
}  // namespace GameEngine
