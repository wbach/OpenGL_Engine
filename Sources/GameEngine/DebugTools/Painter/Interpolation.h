#pragma once
#include <Types.h>

namespace GameEngine
{
using Interpolation = std::function<float(const vec2i&, const vec2i&, uint32)>;

enum class InterpolationType
{
    Hard,
    Linear,
    Smooth,
    Gaussian,
    ConstValue
};

Interpolation makeInterpolation(InterpolationType);
}  // namespace GameEngine
