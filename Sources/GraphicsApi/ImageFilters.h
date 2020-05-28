#pragma once
#include <Types.h>
#include <optional>
#include <vector>

namespace GraphicsApi
{
struct ImageFilter
{
    const std::vector<float>& kernel;
    vec2ui size;

    std::optional<float> getSample(const vec2ui&) const;
    std::optional<float> getSample(uint32, uint32) const;
};

ImageFilter gaussian3x3Filter();
ImageFilter gaussian5x5Filter();
ImageFilter gaussian7x7Filter();
}  // namespace GraphicsApi
