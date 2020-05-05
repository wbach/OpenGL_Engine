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

    vec2ui Size() const;
};
std::optional<Color> GetPixel(Image&, const vec2ui&);
void SetPixel(Image&, const vec2ui&, const Color&);
}  // namespace GameEngine
