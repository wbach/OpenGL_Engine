#pragma once
#include <Types.h>

namespace GameEngine
{
struct PerTerrain
{
    vec4 displacementStrength;
    vec4i morpharea1_4;
    vec4i morpharea5_8;
    vec3 scale;
};
}  // namespace GameEngine