#include "Frustrum.h"
#include <Glm.h>
#include <Logger/Log.h>
#include <algorithm>
#include "Utils.h"

namespace GameEngine
{
namespace
{
uint64 intersectionCheckCount{0};
}
void Frustrum::prepareFrame(const mat4& projectionViewMatrix)
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

    for (auto& plane : planes_)
        plane.Normalize();

    intersectionCheckCount = 0;
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

bool Frustrum::intersection(const vec3& point) const
{
    ++intersectionCheckCount;
    return std::all_of(planes_.begin(), planes_.end(),
                       [&point](const auto& plane) { return ClassifyPoint(plane, point) != Halfspace::NEGATIVE; });
}

bool Frustrum::intersection(const vec3& center, float radius) const
{
    ++intersectionCheckCount;

    for (const auto& plane : planes_)
    {
        if (glm::dot(center, plane.normal_) + plane.d_ + radius <= 0)
            return false;
    }
    return true;
}

bool Frustrum::intersection(const BoundingBox& boundingBox) const
{
    ++intersectionCheckCount;

    constexpr float bias = 0.05f;

    for (const auto& plane : planes_)
    {
        vec3 axisVert;

        if (plane.normal_.x < 0.0f)
            axisVert.x = boundingBox.min().x - bias;
        else
            axisVert.x = boundingBox.max().x + bias;

        if (plane.normal_.y < 0.0f)
            axisVert.y = boundingBox.min().y - bias;
        else
            axisVert.y = boundingBox.max().y + bias;

        if (plane.normal_.z < 0.0f)
            axisVert.z = boundingBox.min().z - bias;
        else
            axisVert.z = boundingBox.max().z + bias;

        if (glm::dot(plane.normal_, axisVert) + plane.d_ < 0.0f)
        {
            return false;
        }
    }
    return true;
}

uint64 Frustrum::getIntersectionsCountInFrame() const
{
    return intersectionCheckCount;
}
}  // namespace GameEngine
