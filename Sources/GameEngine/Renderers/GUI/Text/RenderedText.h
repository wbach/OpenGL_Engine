#pragma once
#include <Types.h>
#include "Surface.h"

namespace GameEngine
{
struct RenderedText
{
    uint32 fontId;
    std::string text;
    vec4 color;
    uint32 outline;
    Surface surface;
    uint32 insances;
};
}  // namespace GameEngine
