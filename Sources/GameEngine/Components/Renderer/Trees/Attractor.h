#pragma once
#include <Types.h>

namespace GameEngine
{
struct Attractor
{
    vec3 position;
    bool reached{false};

    std::optional<int> killerBranch;
};
}  // namespace GameEngine