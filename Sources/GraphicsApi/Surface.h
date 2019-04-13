#pragma once
#include "Types.h"
#include <functional>

namespace GraphicsApi
{
struct Surface
{
    size_t id;
    vec2ui size;
    uint8 bytesPerPixel;
    void* pixels;
};
}  // namespace GraphicsApi
