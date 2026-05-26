#pragma once
#include <limits>

namespace GameEngine
{
struct NavNode
{
    int x{0}, y{0};
    int obstacleCount = 0;
    float height{0.f};

    float gCost{std::numeric_limits<float>::max()};
    float hCost{0.f};
    NavNode* parent{nullptr};

    float fCost() const
    {
        return gCost + hCost;
    }

    bool IsWalkable() const
    {
        return obstacleCount == 0;
    }
};
}  // namespace GameEngine