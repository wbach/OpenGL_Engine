#pragma once
#include <vector>
#include "Types.h"

namespace GameEngine
{
struct Image
{
    std::vector<uint8> data;
    std::vector<float> floatData;

    uint32 width{0};
    uint32 height{0};

    vec2ui Size() const
    {
        return vec2ui(width, height);
    }
};
}  // namespace GameEngine
