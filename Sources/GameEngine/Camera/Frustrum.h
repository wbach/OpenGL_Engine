#pragma once
#include "Plane.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include <array>

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
    bool SphereIntersection(const vec3& center, float radius) const;
    bool aabbIntersection(const BoundingBox&) const;

private:
    std::array<Plane, 6> planes_;
};
}  // namespace GameEngine
