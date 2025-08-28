#include <gtest/gtest.h>
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Renderers/GUI/Text/FontManager.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
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
        sut_ = std::make_unique<GuiTextElement>(fontManager_, guiRenderer_, resourcesManager_, fontStr);
    }

    GraphicsApi::GraphicsApiMock graphicsApi_;
    GUIRenderer guiRenderer_;
    ResourceManagerMock resourcesManager_;
    FontManager fontManager_;
    std::unique_ptr<GuiTextElement> sut_;
};
}  // namespace MT
}  // namespace GameEngine
