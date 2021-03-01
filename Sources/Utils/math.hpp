#pragma once
#include "Types.h"
#include <algorithm>

namespace Utils
{
template <typename T>
T clamp(const T& n, const T& lower, const T& upper)
{
    return std::max(lower, std::min(n, upper));
}

template <class type>
type KmToMs(type a)
{
    return a * 0.277777778f;
}
}
