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

    std::tuple<vec2, vec2, vec2, vec2> init(const vec2& position, const vec2& scale)
    {
        sut_.SetLocalScale(scale);
        sut_.SetLocalPostion(position);

        vec2 halfScale(scale / 2.f);
        vec2 lbCorner = position - halfScale;
        vec2 ltCorner = position + vec2(-halfScale.x, halfScale.y);
        vec2 rtCorner = position + halfScale;
        vec2 rbCorner = position + vec2(halfScale.x, -halfScale.y);
        return {lbCorner, ltCorner, rtCorner, rbCorner};
    }

    vec2 convertToMousePosition(const vec2& mousePos)
    {
        return mousePos * 2.f - 1.f;
    }

    GuiElement sut_;
};

TEST_F(GuiElementShould, IsCollision)
{
    vec2 scale(0.25f, 0.25f);
    vec2 position(0.13, -0.21);
    auto [lbCorner, ltCorner, rtCorner, rbCorner] = init(position, scale);

    {
        vec2 mousePosition = convertToMousePosition(lbCorner + BIAS);
        EXPECT_TRUE(sut_.IsCollision(mousePosition));
    }

    {
        vec2 mousePosition = convertToMousePosition(position);
        EXPECT_TRUE(sut_.IsCollision(mousePosition));
    }

    {
        vec2 mousePosition = convertToMousePosition(ltCorner + vec2(BIAS, -BIAS));
        EXPECT_TRUE(sut_.IsCollision(mousePosition));
    }

    {
        vec2 mousePosition = convertToMousePosition(rtCorner - BIAS);
        EXPECT_TRUE(sut_.IsCollision(mousePosition));
    }

    {
        vec2 mousePosition = convertToMousePosition(rbCorner + vec2(-BIAS, BIAS));
        EXPECT_TRUE(sut_.IsCollision(mousePosition));
    }
}

TEST_F(GuiElementShould, NoCollisions)
{
    vec2 scale(0.25f, 0.25f);
    vec2 position(-0.8, 0.56);

    auto [lbCorner, ltCorner, rtCorner, rbCorner] = init(position, scale);

    {
        vec2 mousePosition = convertToMousePosition(lbCorner - BIAS);
        EXPECT_FALSE(sut_.IsCollision(mousePosition));
    }

    {
        vec2 mousePosition = convertToMousePosition(ltCorner + BIAS);
        EXPECT_FALSE(sut_.IsCollision(mousePosition));
    }

    {
        vec2 mousePosition = convertToMousePosition(rtCorner + BIAS);
        EXPECT_FALSE(sut_.IsCollision(mousePosition));
    }

    {
        vec2 mousePosition = convertToMousePosition(rbCorner + BIAS);
        EXPECT_FALSE(sut_.IsCollision(mousePosition));
    }
}
}  // namespace UT
}  // namespace GameEngine
