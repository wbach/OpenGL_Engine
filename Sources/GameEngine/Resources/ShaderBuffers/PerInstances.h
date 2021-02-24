#pragma once
#include "Types.h"
#include <array>

namespace GameEngine
{
const int MAX_INSTANCES = 512;

struct PerInstances
{
    std::array<mat4, MAX_INSTANCES> transforms;

    PerInstances()
    {
        clear();
    }

    void clear()
    {
        for (auto& t : transforms)
            t = mat4(1.f);
    }
};
}  // namespace GameEngine
