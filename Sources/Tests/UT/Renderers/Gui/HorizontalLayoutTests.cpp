#include <GameEngine/Renderers/GUI/Element.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <gtest/gtest.h>

#include "Tests/Mocks/Api/InputManagerMock.h"

namespace GameEngine
{
namespace UT
{
struct GuiHorizontalLayoutTests : public ::testing::Test
{
    void createElement(const vec2& scale)
    {
        auto element = std::make_unique<GUI::Element>();
        element->setLocalScale(scale);
        sut_.addChild(std::move(element));
        sut_.refresh();
    }

    Input::InputManagerMock inputManager_;
    GUI::HorizontalLayout sut_;
};

TEST_F(GuiHorizontalLayoutTests, HorizontalLayoutPositioningLocalScaleAlignCenterOneElement)
{
    sut_.setAlign(GameEngine::GUI::HorizontalAlign::CENTER);
    sut_.setScreenScale({1.f, 1.f});
    createElement({0.2f, 0.05f});

    sut_.refresh();

    const auto& children = sut_.getChildren();
    auto& localPosition  = children[0]->getLocalPosition();
    EXPECT_FLOAT_EQ(localPosition.x, 0.5f);
}

TEST_F(GuiHorizontalLayoutTests, HorizontalLayoutPositioningLocalScaleAlignCenterTwoElement)
{
    sut_.setAlign(GameEngine::GUI::HorizontalAlign::CENTER);
    sut_.setScreenScale({1.f, 1.f});

    vec2 childScale1{0.2f, 0.05f};
    vec2 childScale2{0.4f, 0.05f};
    createElement(childScale1);
    createElement(childScale2);

    float halfSumChildX = (childScale1.x + childScale2.x) / 2.f;

    const auto& children = sut_.getChildren();
    EXPECT_FLOAT_EQ(children[0]->getLocalPosition().x, 0.5f - halfSumChildX + 0.5f * childScale1.x);
    EXPECT_FLOAT_EQ(children[1]->getLocalPosition().x, 0.5f - halfSumChildX + childScale1.x + 0.5f * childScale2.x);
}

TEST_F(GuiHorizontalLayoutTests, ChildHorizontalLayoutPosAlignCenterTwoElement)
{
    const auto& children = sut_.getChildren();

    GUI::Element parent;
    parent.setLocalScale({0.5f, 0.5f});
    sut_.setParent(&parent);

    sut_.setAlign(GameEngine::GUI::HorizontalAlign::CENTER);
    sut_.setLocalScale({1.f, 1.f});

    EXPECT_FLOAT_EQ(sut_.getScreenScale().x, 0.5f);
    EXPECT_FLOAT_EQ(sut_.getScreenScale().y, 0.5f);

    vec2 childLocalScale1{0.2f, 0.05f};
    vec2 childLocalScale2{0.4f, 0.05f};

    createElement(childLocalScale1);
    EXPECT_FLOAT_EQ(children[0]->getScreenPosition().x, 0.5f);
    createElement(childLocalScale2);

    float halfSumChildX = ((childLocalScale1.x + childLocalScale2.x) / 2.f) * parent.getScreenScale().x;
    EXPECT_FLOAT_EQ(children[0]->getScreenPosition().x, 0.5f - halfSumChildX + (children[0]->getScreenScale().x / 2.f));
    EXPECT_FLOAT_EQ(children[1]->getScreenPosition().x,
                    0.5f - halfSumChildX + children[0]->getScreenScale().x + (children[1]->getScreenScale().x / 2.f));
}

TEST_F(GuiHorizontalLayoutTests, HorizontalLayoutPositioningLocalScaleAlignLeftOneElement)
{
    sut_.setParent(nullptr);
    sut_.setAlign(GameEngine::GUI::HorizontalAlign::LEFT);
    sut_.setScreenScale({1.f, 1.f});
    createElement({0.2f, 0.05f});

    const auto& children = sut_.getChildren();
    auto& localPosition  = children[0]->getLocalPosition();
    EXPECT_FLOAT_EQ(localPosition.x, 0.1f);
}
TEST_F(GuiHorizontalLayoutTests, HorizontalLayoutPositioningLocalScaleAlignRightOneElement)
{
    sut_.setParent(nullptr);
    sut_.setAlign(GameEngine::GUI::HorizontalAlign::RIGHT);
    sut_.setScreenScale({1.f, 1.f});
    createElement({0.2f, 0.05f});

    const auto& children = sut_.getChildren();
    auto& localPosition  = children[0]->getLocalPosition();
    EXPECT_FLOAT_EQ(localPosition.x, 0.9f);
}
}  // namespace UT
}  // namespace GameEngine
