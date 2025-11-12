#pragma once
#include <Types.h>

namespace GameEngine
{
struct GrassShaderBuffer
{
    AlignWrapper<vec4> windParams; // x = windStrength, y = windFrequency, z = windSpeed, w = unused
    AlignWrapper<vec4> variables;  // x = viewDistance, y - globalTime, z = windDirX, w = windDirZ
};
}  // namespace GameEngine