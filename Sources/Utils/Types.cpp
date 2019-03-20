#include "Types.h"

namespace wb
{
std::string to_string(const vec2i& v)
{
    return std::to_string(v.x) + " " + std::to_string(v.y);
}

std::string to_string(const vec3i& v)
{
    return std::to_string(v.x) + " " + std::to_string(v.y) + " " + std::to_string(v.z);
}
}  // namespace wb

vec4 ToVec4(const vec3& v3)
{
    return vec4(v3.x, v3.y, v3.z, 1.0f);
}

vec4 ToVec4(const vec3& v3, float w)
{
    return vec4(v3.x, v3.y, v3.z, w);
}

vec4 ToVec4(const vec2& v2, float z, float w)
{
    return vec4(v2.x, v2.y, z, w);
}