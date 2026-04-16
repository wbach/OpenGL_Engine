#pragma once

namespace GameEngine
{
namespace GUI
{
struct Spacing
{
    float top{0.f};
    float bottom{0.f};
    float left{0.f};
    float right{0.f};

    Spacing() = default;

    Spacing(float all)
        : top(all)
        , bottom(all)
        , left(all)
        , right(all)
    {
    }
    Spacing(float vertical, float horizontal)
        : top(vertical)
        , bottom(vertical)
        , left(horizontal)
        , right(horizontal)
    {
    }
};
}  // namespace GUI
}  // namespace GameEngine
