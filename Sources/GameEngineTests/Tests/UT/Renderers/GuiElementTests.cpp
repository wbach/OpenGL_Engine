#include <gtest/gtest.h>
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace GameEngine
{
namespace UT
{
const float BIAS = 0.001f;

struct GuiElementShould : public ::testing::Test
{
    GuiElementShould()
        : sut_(GuiElementTypes::Button)
    {
    }

    GuiElement sut_;
};

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
}  // namespace UT
}  // namespace GameEngine
