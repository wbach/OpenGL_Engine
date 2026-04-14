#include <GameEngine/Renderers/GUI/Element.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <Logger/Log.h>
#include <gtest/gtest.h>

namespace GameEngine
{
namespace UT
{
struct GuiVerticalLayoutTests : public ::testing::Test
{
    GuiVerticalLayoutTests()
        : sut_()
    {
    }

    void createElement(const vec2& scale)
    {
        auto element = std::make_unique<GUI::Element>();
        element->setLocalScale(scale);
        sut_.addChild(std::move(element));
    }

    GUI::VerticalLayout sut_;
};

TEST_F(GuiVerticalLayoutTests, VerticalAlignCenter)
{
    sut_.setAlign(GameEngine::GUI::HorizontalAlign::CENTER);
    sut_.setScreenScale({1.f, 1.f});
    createElement({0.2f, 0.05f});
    createElement({0.2f, 0.2f});

    const auto& children = sut_.getChildren();
    EXPECT_TRUE(children[0]->isActive());
    EXPECT_TRUE(children[1]->isActive());
    EXPECT_FLOAT_EQ(children[0]->getLocalPosition().y, 0.975f);
    EXPECT_FLOAT_EQ(children[1]->getLocalPosition().y, 1.0f - 0.05f - 0.1f);
}
TEST_F(GuiVerticalLayoutTests, VerticalAlignCenterWithParent)
{
    GUI::Element parent;
    parent.setLocalScale({0.1, 0.5f});
    parent.setLocalPosition({0.3f, 0.3f});

    sut_.setParent(&parent);
    sut_.setAlign(GameEngine::GUI::HorizontalAlign::CENTER);
    sut_.setLocalScale({1.f, 1.f});

    createElement({0.2f, 0.05f});
    createElement({0.2f, 0.2f});

    const auto& children = sut_.getChildren();
    EXPECT_TRUE(children[0]->isActive());
    EXPECT_TRUE(children[1]->isActive());

    EXPECT_FLOAT_EQ(children[0]->getLocalPosition().y, 0.975f);
    EXPECT_FLOAT_EQ(children[1]->getLocalPosition().y, 1.0f - 0.05f - 0.1f);

    auto parentPositionY       = parent.getScreenPosition().y;
    auto screenScaleY          = 0.25f;
    auto screenScaleChild1     = children[0]->getScreenScale().y;
    auto halfScreenScaleChild1 = screenScaleChild1 / 2.f;
    auto halfScreenScaleChild2 = 0.05f;

    EXPECT_FLOAT_EQ(children[0]->getScreenPosition().y, parentPositionY + screenScaleY - halfScreenScaleChild1);
    EXPECT_FLOAT_EQ(children[1]->getScreenPosition().y,
                    parentPositionY + screenScaleY - screenScaleChild1 - halfScreenScaleChild2);
}
TEST_F(GuiVerticalLayoutTests, VerticalAlignCenterWithParentPauseMenuIssue)
{
    GUI::Element parent;
    parent.setLocalScale({0.1f, 1.f});
    parent.setLocalPosition({0.25f, 0.f});

    sut_.setParent(&parent);
    sut_.setAlign(GameEngine::GUI::HorizontalAlign::CENTER);
    sut_.setLocalScale({1.f, 0.5f});
    sut_.setLocalPosition({0.f, 0.25f});

    createElement({1.f, 0.05f});
    createElement({0.2f, 0.2f});

    LOG_DEBUG << "refresh";
    sut_.refresh();

    const auto& children = sut_.getChildren();
    EXPECT_TRUE(children[0]->isActive());
    EXPECT_TRUE(children[1]->isActive());

    EXPECT_FLOAT_EQ(children[0]->getLocalPosition().y, 0.975f);
    EXPECT_FLOAT_EQ(children[1]->getLocalPosition().y, 1.0f - 0.05f - 0.1f);

    // EXPECT_FLOAT_EQ(children[0]->GetScreenPosition().y, 0.25f + 0.25f - 0.0125f );
    // EXPECT_FLOAT_EQ(children[1]->GetScreenPosition().y, 0.25f + 0.25f - 0.025f - 0.05f);

    EXPECT_FLOAT_EQ(children[0]->getScreenScale().x, 0.1f);
    EXPECT_FLOAT_EQ(children[0]->getScreenScale().y, 0.025f);
}
}  // namespace UT
}  // namespace GameEngine
