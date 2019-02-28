#pragma once
#include "Types.h"

namespace DirectX
{
struct Vertex
{
    vec3 position;
    vec2 textCoord;
    vec3 normal;
    vec3 tangent;
    //vec3 weights;
    //vec3i bonesIds;
};
}  // namespace DirectX
