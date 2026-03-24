#pragma once
#include <limits>

namespace GameEngine
{
struct NavNode
{
    int x{0}, y{0};
    bool isWalkable{true};
    float height{0.f};

    float gCost{std::numeric_limits<float>::max()};
    float hCost{0.f};
    NavNode* parent{nullptr};

    float fCost() const
    {
        return gCost + hCost;
    }
};
}  // namespace GameEngine