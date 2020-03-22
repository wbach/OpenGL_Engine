#pragma once
#include <Types.h>

namespace GameEngine
{
struct GrassShaderBuffer
{
    AlignWrapper<vec2> variables;  // x = viewDistance, y - globalTime
};
}  // namespace GameEngine