#pragma once
#include <stdint.h>

namespace GraphicsApi
{
enum class RenderType : uint8_t
{
    TRIANGLES,
    TRIAGNLE_STRIP,
    PATCHES,
    POINTS,
    LINES
};
}  // namespace GraphicsApi
