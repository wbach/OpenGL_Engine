#pragma once
#include "Types.h"

namespace Utils
{
std::vector<vec2> PoissonDiskSample(const vec2&, const vec2&, float, int = 30);
}  // namespace Utils
