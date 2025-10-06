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
Frustrum::Frustrum()
{
    planes_.emplace_back();
}
void Frustrum::prepareFrame(const mat4& projectionViewMatrix)
{
    auto& planes  = planes_.back();
    const auto& m = projectionViewMatrix;
    // Left clipping plane
    planes[0].normal_.x = m[0][3] + m[0][0];
    planes[0].normal_.y = m[1][3] + m[1][0];
    planes[0].normal_.z = m[2][3] + m[2][0];
    planes[0].d_        = m[3][3] + m[3][0];
    // Right clipping plane
    planes[1].normal_.x = m[0][3] - m[0][0];
    planes[1].normal_.y = m[1][3] - m[1][0];
    planes[1].normal_.z = m[2][3] - m[2][0];
    planes[1].d_        = m[3][3] - m[3][0];
    // Top clipping plane
    planes[2].normal_.x = m[0][3] - m[0][1];
    planes[2].normal_.y = m[1][3] - m[1][1];
    planes[2].normal_.z = m[2][3] - m[2][1];
    planes[2].d_        = m[3][3] - m[3][1];
    // Bottom clipping plane
    planes[3].normal_.x = m[0][3] + m[0][1];
    planes[3].normal_.y = m[1][3] + m[1][1];
    planes[3].normal_.z = m[2][3] + m[2][1];
    planes[3].d_        = m[3][3] + m[3][1];
    // Near clipping plane
    planes[4].normal_.x = m[0][3] + m[0][2];
    planes[4].normal_.y = m[1][3] + m[1][2];
    planes[4].normal_.z = m[2][3] + m[2][2];
    planes[4].d_        = m[3][3] + m[3][2];
    // Far clipping plane
    planes[5].normal_.x = m[0][3] - m[0][2];
    planes[5].normal_.y = m[1][3] - m[1][2];
    planes[5].normal_.z = m[2][3] - m[2][2];
    planes[5].d_        = m[3][3] - m[3][2];

    for (auto& plane : planes)
        plane.Normalize();

    intersectionCheckCount = 0;
}

Halfspace ClassifyPoint(const Plane& plane, const vec3& pt)
{
    float scale = glm::length(pt);

    // float przy wiekszych wartosciach trafil precyzje i zbyt maly epsilon nie dzialal
    constexpr float EPSILON_FACTOR = 0.0001f;
    float epsilon                  = scale * EPSILON_FACTOR;

    float distance = (plane.normal_.x * pt.x) + (plane.normal_.y * pt.y) + (plane.normal_.z * pt.z) + plane.d_;
    if (distance < -epsilon)
        return Halfspace::NEGATIVE;
    if (distance > epsilon)
        return Halfspace::POSITIVE;

    return Halfspace::ON_PLANE;
}

bool Frustrum::intersection(const vec3& point) const
{
    ++intersectionCheckCount;
    return std::all_of(planes_.back().begin(), planes_.back().end(),
                       [&point](const auto& plane) { return ClassifyPoint(plane, point) != Halfspace::NEGATIVE; });
}

bool Frustrum::intersection(const vec3& center, float radius) const
{
    ++intersectionCheckCount;

    for (const auto& plane : planes_.back())
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

    for (const auto& plane : planes_.back())
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
void Frustrum::push(const mat4& projectionViewMatrix)
{
    planes_.emplace_back();
    prepareFrame(projectionViewMatrix);
}
void Frustrum::pop()
{
    if (planes_.size() > 1)
        planes_.pop_back();
}
const std::vector<std::array<Plane, 6>>& Frustrum::getPlanes() const
{
    return planes_;
}
}  // namespace GameEngine
