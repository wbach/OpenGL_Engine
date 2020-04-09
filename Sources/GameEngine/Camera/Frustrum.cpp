#include "Frustrum.h"
#include <Logger/Log.h>
#include "Utils.h"
#include <Glm.h>
#include <algorithm>

namespace GameEngine
{
void Frustrum::CalculatePlanes(const mat4& projectionViewMatrix)
{
    const auto& m = projectionViewMatrix;
    // Left clipping plane
    planes_[0].normal_.x = m[0][3] + m[0][0];
    planes_[0].normal_.y = m[1][3] + m[1][0];
    planes_[0].normal_.z = m[2][3] + m[2][0];
    planes_[0].d_        = m[3][3] + m[3][0];
    // Right clipping plane
    planes_[1].normal_.x = m[0][3] - m[0][0];
    planes_[1].normal_.y = m[1][3] - m[1][0];
    planes_[1].normal_.z = m[2][3] - m[2][0];
    planes_[1].d_        = m[3][3] - m[3][0];
    // Top clipping plane
    planes_[2].normal_.x = m[0][3] - m[0][1];
    planes_[2].normal_.y = m[1][3] - m[1][1];
    planes_[2].normal_.z = m[2][3] - m[2][1];
    planes_[2].d_        = m[3][3] - m[3][1];
    // Bottom clipping plane
    planes_[3].normal_.x = m[0][3] + m[0][1];
    planes_[3].normal_.y = m[1][3] + m[1][1];
    planes_[3].normal_.z = m[2][3] + m[2][1];
    planes_[3].d_        = m[3][3] + m[3][1];
    // Near clipping plane
    planes_[4].normal_.x = m[0][3] + m[0][2];
    planes_[4].normal_.y = m[1][3] + m[1][2];
    planes_[4].normal_.z = m[2][3] + m[2][2];
    planes_[4].d_        = m[3][3] + m[3][2];
    // Far clipping plane
    planes_[5].normal_.x = m[0][3] - m[0][2];
    planes_[5].normal_.y = m[1][3] - m[1][2];
    planes_[5].normal_.z = m[2][3] - m[2][2];
    planes_[5].d_        = m[3][3] - m[3][2];

    for(auto& plane : planes_)
        plane.Normalize();
}

Halfspace ClassifyPoint(const Plane& plane, const vec3& pt)
{
    float distance = (plane.normal_.x * pt.x) + (plane.normal_.y * pt.y) + (plane.normal_.z * pt.z) + plane.d_;
    if (distance < 0)
        return Halfspace::NEGATIVE;
    if (distance > 0)
        return Halfspace::POSITIVE;

    return Halfspace::ON_PLANE;
}

bool Frustrum::PointIntersection(const vec3& point) const
{
    return std::all_of(planes_.begin(), planes_.end(), [&point](const auto& plane) { return ClassifyPoint(plane, point) != Halfspace::NEGATIVE; });
}

bool Frustrum::SphereIntersection(const vec3& center, float radius) const
{
    for (const auto& plane : planes_)
    {
        // Plane-sphere intersection test. If p*n + d + r < 0 then we're outside the plane.
        if (glm::dot(center, plane.normal_) + plane.d_ + radius <= 0)
            return false;
    }
    return true;
}
}  // namespace GameEngine
