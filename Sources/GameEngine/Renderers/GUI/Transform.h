#pragma once
#include <Types.h>
#include <ostream>
namespace GameEngine
{
namespace GUI
{
struct Transform
{
    vec2 position{0.5f};
    vec2 scale{1.0f};
    float zValue{0.f};

    friend std::ostream& operator<<(std::ostream&, const Transform&);
};
}  // namespace GUI
}  // namespace GameEngine
