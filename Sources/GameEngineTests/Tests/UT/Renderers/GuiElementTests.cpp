#include <gtest/gtest.h>
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace GameEngine
{
namespace UT
{
const vec2ui WINDOW_SIZE{640, 480};
const float BIAS = 0.001f;

struct GuiElementShould : public ::testing::Test
{
    GuiElementShould()
        : sut_(GuiElementTypes::Button, WINDOW_SIZE)
    {
    }

    GuiElement sut_;
};

TEST_F(GuiElementShould, CalculatePositionBasedOnRect_0_0)
{
    Rect rect{0, 0, 640 , 300};
    sut_.SetRect(rect);

    const auto& position = sut_.GetPosition();
    EXPECT_FLOAT_EQ(position.x, -1.f);
    EXPECT_FLOAT_EQ(position.y, -1.f);
}

TEST_F(GuiElementShould, CalculatePositionBasedOnRect_05_05)
{
    Rect rect{WINDOW_SIZE.x / 2, WINDOW_SIZE.y / 2, 640 , 300};
    sut_.SetRect(rect);

    const auto& position = sut_.GetPosition();
    EXPECT_FLOAT_EQ(position.x, 0.f);
    EXPECT_FLOAT_EQ(position.y, 0.f);
}

TEST_F(GuiElementShould, CollisionsFloatCenteredPosition)
{
    vec2 scale(0.25f, 0.25f);
    vec2 position(0.13, -0.21);

    sut_.SetScale(scale);
    sut_.SetPostion(position);

    {
        vec2 mousePosition{-scale.x + BIAS, -scale.y + BIAS};
        EXPECT_TRUE(sut_.IsCollision(mousePosition + position));
    }

    {
        vec2 mousePosition{0, 0};
        EXPECT_TRUE(sut_.IsCollision(mousePosition + position));
    }

    {
        vec2 mousePosition{scale.x - BIAS, -scale.y + BIAS};
        EXPECT_TRUE(sut_.IsCollision(mousePosition + position));
    }

    {
        vec2 mousePosition{-scale.x + BIAS, scale.y - BIAS};
        EXPECT_TRUE(sut_.IsCollision(mousePosition + position));
    }

    {
        vec2 mousePosition{scale.x - BIAS, scale.y - BIAS};
        EXPECT_TRUE(sut_.IsCollision(mousePosition + position));
    }
}

TEST_F(GuiElementShould, NoCollisionsFloatCenteredPosition)
{
    vec2 scale(0.25f, 0.25f);
    vec2 position(-0.8, 0.56);

    sut_.SetScale(scale);
    sut_.SetPostion(position);

    {
        vec2 mousePosition{-scale.x - BIAS, -scale.y - BIAS};
        EXPECT_FALSE(sut_.IsCollision(mousePosition + position));
    }

    {
        vec2 mousePosition{scale.x + BIAS, -scale.y - BIAS};
        EXPECT_FALSE(sut_.IsCollision(mousePosition + position));
    }

    {
        vec2 mousePosition{-scale.x - BIAS, scale.y + BIAS};
        EXPECT_FALSE(sut_.IsCollision(mousePosition + position));
    }

    {
        vec2 mousePosition{scale.x + BIAS, scale.y + BIAS};
        EXPECT_FALSE(sut_.IsCollision(mousePosition + position));
    }
}

TEST_F(GuiElementShould, CollisionsFloatCustomPositions)
{

}

}  // namespace UT
}  // namespace GameEngine
