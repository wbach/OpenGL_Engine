#pragma once
#include <Types.h>

namespace GameEngine
{
class ICamera;

vec3 CalculateMouseRayDirection(const ICamera&, const vec2&);
}  // namespace GameEngine
