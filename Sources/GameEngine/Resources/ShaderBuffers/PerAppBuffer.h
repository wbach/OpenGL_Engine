#pragma once
#include "Types.h"

namespace GameEngine
{
struct PerAppBuffer
{
    AlignWrapper<vec4> useTextures; // x - diffuse, y - normal, z - specular, w - displacement
    AlignWrapper<vec4> viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    AlignWrapper<vec4> shadowVariables;
    AlignWrapper<vec4> clipPlane;
};
}  // namespace GameEngine
