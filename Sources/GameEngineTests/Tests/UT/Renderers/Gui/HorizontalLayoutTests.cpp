#include <GameEngine/Renderers/GUI/GuiElement.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <gtest/gtest.h>

#include "GameEngineTests/Tests/Mocks/Api/InputManagerMock.h"

namespace GameEngine
{
namespace UT
{
const float BIAS = 0.001f;

struct GuiHorizontalLayoutTests : public ::testing::Test
{
    GuiHorizontalLayoutTests()
        : sut_(inputManager_)
    {
    }

    void createElement(const vec2& scale)
    {
        auto element = std::make_unique<GuiElement>(GuiElementTypes::Button);
        element->SetLocalScale(scale);
        sut_.AddChild(std::move(element));
    }

    Input::InputManagerMock inputManager_;
    HorizontalLayout sut_;
};

TEST_F(GuiHorizontalLayoutTests, HorizontalLayoutPositioningLocalScaleAlginCenterOneElement)
{
    sut_.SetAlgin(GameEngine::Layout::Algin::CENTER);
    sut_.SetScreenScale({1.f, 1.f});
    createElement({0.2f, 0.05f});

    const auto& children = sut_.GetChildren();
    auto& localPosition  = children[0]->GetLocalPosition();
    EXPECT_FLOAT_EQ(localPosition.x, 0.5f);
}

TEST_F(GuiHorizontalLayoutTests, HorizontalLayoutPositioningLocalScaleAlginCenterTwoElement)
{
    sut_.SetAlgin(GameEngine::Layout::Algin::CENTER);
    sut_.SetScreenScale({1.f, 1.f});

    vec2 childScale1{0.2f, 0.05f};
    vec2 childScale2{0.4f, 0.05f};
    createElement(childScale1);
    createElement(childScale2);

    float halfSumChildX = (childScale1.x + childScale2.x) / 2.f;

    const auto& children = sut_.GetChildren();
    EXPECT_FLOAT_EQ(children[0]->GetLocalPosition().x, 0.5f - halfSumChildX + 0.5f * childScale1.x);
    EXPECT_FLOAT_EQ(children[1]->GetLocalPosition().x, 0.5f - halfSumChildX + childScale1.x + 0.5f * childScale2.x);
}

TEST_F(GuiHorizontalLayoutTests, ChildHorizontalLayoutPosAlginCenterTwoElement)
{
    const auto& children = sut_.GetChildren();

    GuiElement parent(GameEngine::GuiElementTypes::Window);
    parent.SetLocalScale({0.5f, 0.5f});
    sut_.setParent(&parent);

    sut_.SetAlgin(GameEngine::Layout::Algin::CENTER);
    sut_.SetLocalScale({1.f, 1.f});

    EXPECT_FLOAT_EQ(sut_.GetScreenScale().x, 0.5f);
    EXPECT_FLOAT_EQ(sut_.GetScreenScale().y, 0.5f);

    vec2 childLocalScale1{0.2f, 0.05f};
    vec2 childLocalScale2{0.4f, 0.05f};

    createElement(childLocalScale1);
    EXPECT_FLOAT_EQ(children[0]->GetScreenPosition().x, 0.5f);
    createElement(childLocalScale2);

    float halfSumChildX = ((childLocalScale1.x + childLocalScale2.x) / 2.f) * parent.GetScreenScale().x;
    EXPECT_FLOAT_EQ(children[0]->GetScreenPosition().x, 0.5f - halfSumChildX + (children[0]->GetScreenScale().x / 2.f));
    EXPECT_FLOAT_EQ(children[1]->GetScreenPosition().x, 0.5f -halfSumChildX + children[0]->GetScreenScale().x + (children[1]->GetScreenScale().x / 2.f));
}

TEST_F(GuiHorizontalLayoutTests, HorizontalLayoutPositioningLocalScaleAlginLeftOneElement)
{
    sut_.setParent(nullptr);
    sut_.SetAlgin(GameEngine::Layout::Algin::LEFT);
    sut_.SetScreenScale({1.f, 1.f});
    createElement({0.2f, 0.05f});

    const auto& children = sut_.GetChildren();
    auto& localPosition  = children[0]->GetLocalPosition();
    EXPECT_FLOAT_EQ(localPosition.x, 0.1f);
}
TEST_F(GuiHorizontalLayoutTests, HorizontalLayoutPositioningLocalScaleAlginRightOneElement)
{
    sut_.setParent(nullptr);
    sut_.SetAlgin(GameEngine::Layout::Algin::RIGHT);
    sut_.SetScreenScale({1.f, 1.f});
    createElement({0.2f, 0.05f});

    const auto& children = sut_.GetChildren();
    auto& localPosition  = children[0]->GetLocalPosition();
    EXPECT_FLOAT_EQ(localPosition.x, 0.9f);
}
}  // namespace UT
}  // namespace GameEngine
