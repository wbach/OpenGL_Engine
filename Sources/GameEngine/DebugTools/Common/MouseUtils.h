#pragma once
#include <Types.h>

namespace GameEngine
{
class Projection;
class CameraWrapper;

vec3 CalculateMouseRay(const Projection&, const CameraWrapper&, const vec2&);
}  // namespace GameEngine
