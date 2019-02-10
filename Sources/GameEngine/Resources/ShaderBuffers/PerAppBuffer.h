#pragma once
#include "Types.h"

namespace GameEngine
{
struct PerAppBuffer
{
    AlignWrapper<float> useTextures;
    AlignWrapper<float> viewDistance;
    AlignWrapper<vec3> shadowVariables;
    AlignWrapper<vec4> clipPlane;
};
}  // namespace GameEngine
