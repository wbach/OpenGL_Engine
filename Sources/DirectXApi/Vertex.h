#pragma once
#include "Types.h"

namespace DirectX
{
#pragma pack(push, 1)
struct Vertex
{
    vec3 position;
    vec2 textCoord;
    vec3 normal;
    vec3 tangent;
    vec4 weights;
    vec4i bonesIds;
};
#pragma pack(pop)
}  // namespace DirectX
