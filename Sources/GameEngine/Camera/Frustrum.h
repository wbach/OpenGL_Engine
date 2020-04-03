#pragma once
#include "Plane.h"

namespace GameEngine
{
enum class Halfspace
{
    NEGATIVE,
    ON_PLANE,
    POSITIVE
};

class Frustrum
{
public:
    void CalculatePlanes(const mat4& projectionViewMatrix);
    bool PointIntersection(const vec3& point) const;
    bool SphereIntersection(const vec3& center, const float& radius) const;

private:
    std::array<Plane, 6> planes_;
};
}  // namespace GameEngine
