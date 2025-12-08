#pragma once
#include <Types.h>

namespace GameEngine
{
class ICamera;

Ray CalculateMouseRay(const ICamera&, const vec2&);
}  // namespace GameEngine
