#pragma once
#include <functional>
#include "Types.h"

namespace GameEngine
{
struct Surface
{
    size_t id;
    vec2ui size;
    uint8 bytesPerPixel;
    void* pixels;
};
}  // namespace GameEngine
