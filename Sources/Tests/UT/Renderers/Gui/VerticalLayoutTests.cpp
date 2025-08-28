#include <GameEngine/Renderers/GUI/GuiElement.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <gtest/gtest.h>

#include "Tests/Mocks/Api/InputManagerMock.h"

namespace GameEngine
{
namespace UT
{
const float BIAS = 0.001f;

struct GuiVerticalLayoutTests : public ::testing::Test
{
    GuiVerticalLayoutTests()
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
    VerticalLayout sut_;
};

TEST_F(GuiVerticalLayoutTests, VerticalAlginCenter)
{
    sut_.SetAlgin(GameEngine::Layout::Algin::CENTER);
    sut_.SetScreenScale({1.f, 1.f});
    createElement({0.2f, 0.05f});
    createElement({0.2f, 0.2f });

    const auto& children = sut_.GetChildren();
    EXPECT_TRUE(children[0]->IsActive());
    EXPECT_TRUE(children[1]->IsActive());
    EXPECT_FLOAT_EQ(children[0]->GetLocalPosition().y, 0.975f);
    EXPECT_FLOAT_EQ(children[1]->GetLocalPosition().y, 1.0f - 0.05f - 0.1f);
}
TEST_F(GuiVerticalLayoutTests, VerticalAlginCenterWithParent)
{
    GuiElement parent(GuiElementTypes::Window);
    parent.SetLocalScale({ 0.1, 0.5f });
    parent.SetLocalPostion({0.3f, 0.3f});

    sut_.setParent(&parent);
    sut_.SetAlgin(GameEngine::Layout::Algin::CENTER);
    sut_.SetLocalScale({ 1.f, 1.f });

    createElement({ 0.2f, 0.05f });
    createElement({ 0.2f, 0.2f });

    const auto& children = sut_.GetChildren();
    EXPECT_TRUE(children[0]->IsActive());
    EXPECT_TRUE(children[1]->IsActive());
    EXPECT_TRUE(children[0]->IsShow());
    EXPECT_TRUE(children[1]->IsShow());

    EXPECT_FLOAT_EQ(children[0]->GetLocalPosition().y, 0.975f);
    EXPECT_FLOAT_EQ(children[1]->GetLocalPosition().y, 1.0f - 0.05f - 0.1f);

    auto parentPositionY = parent.GetScreenPosition().y;
    auto screenScaleY = 0.25f;
    auto screenScaleChild1 = children[0]->GetScreenScale().y;
    auto halfScreenScaleChild1 = screenScaleChild1 / 2.f;
    auto halfScreenScaleChild2 = 0.05f;

    EXPECT_FLOAT_EQ(children[0]->GetScreenPosition().y, parentPositionY + screenScaleY - halfScreenScaleChild1);
    EXPECT_FLOAT_EQ(children[1]->GetScreenPosition().y, parentPositionY + screenScaleY - screenScaleChild1 - halfScreenScaleChild2);
}
TEST_F(GuiVerticalLayoutTests, VerticalAlginCenterWithParentPauseMenuIssue)
{
    GuiElement parent(GuiElementTypes::Window);
    parent.SetLocalScale({ 0.1f, 1.f });
    parent.SetLocalPostion({ 0.25f, 0.f });

    sut_.setParent(&parent);
    sut_.SetAlgin(GameEngine::Layout::Algin::CENTER);
    sut_.SetLocalScale({ 1.f, 0.5f });
    sut_.SetLocalPostion({ 0.f, 0.25f });

    createElement({ 1.f, 0.05f });
    createElement({ 0.2f, 0.2f });

    const auto& children = sut_.GetChildren();
    EXPECT_TRUE(children[0]->IsActive());
    EXPECT_TRUE(children[1]->IsActive());
    EXPECT_TRUE(children[0]->IsShow());
    EXPECT_TRUE(children[1]->IsShow());

    EXPECT_FLOAT_EQ(children[0]->GetLocalPosition().y, 0.975f);
    EXPECT_FLOAT_EQ(children[1]->GetLocalPosition().y, 1.0f - 0.05f - 0.1f);

    //EXPECT_FLOAT_EQ(children[0]->GetScreenPosition().y, 0.25f + 0.25f - 0.0125f );
    //EXPECT_FLOAT_EQ(children[1]->GetScreenPosition().y, 0.25f + 0.25f - 0.025f - 0.05f);

    EXPECT_FLOAT_EQ(children[0]->GetScreenScale().x, 0.1f);
    EXPECT_FLOAT_EQ(children[0]->GetScreenScale().y, 0.025f);
}
}  // namespace UT
}  // namespace GameEngine
