#pragma once
#include <Types.h>

namespace GameEngine
{
struct PerTerrain
{
    AlignWrapper<vec4> displacementStrength;
    AlignWrapper<vec4i> morpharea1_4;
    AlignWrapper<vec4i> morpharea5_8;
    AlignWrapper<vec4> scaleAndYOffset;
};
}  // namespace GameEngine
