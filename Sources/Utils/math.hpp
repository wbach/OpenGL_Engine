#pragma once
#include "Types.h"

namespace Utils
{
template <typename T>
T clamp(const T& n, const T& lower, const T& upper)
{
    return std::max(lower, std::min(n, upper));
}

template <class type>
type ToRadians(type a)
{
    return a * static_cast<float>(M_PI) / 180.0f;
}

template <class type>
type ToDegrees(type a)
{
    return a * 180.0f / static_cast<float>(M_PI);
}

template <class type>
type KmToMs(type a)
{
    return a * 0.277777778f;
}
}
