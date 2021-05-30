#include "Triangle.h"
#include "../Ray.h"

namespace GameEngine
{
namespace RayTracer
{
Triangle::Triangle(const common::Transform& transform, const Material& material, const vec3& v1, const vec3& v2, const vec3& v3)
    : Triangle(transform, material, v1, v2, v3, calculateStaticNormal(v1, v2, v3))
{
}
Triangle::Triangle(const common::Transform& transform, const Material& material, const vec3& v1, const vec3& v2, const vec3& v3, const vec3& normal)
    : BaseObject(transform, material)
    , v_({v1, v2, v3})
    , useSmoothNormal_(false)
{
    normal_ = normal;
    calculateIntersectionConstVariables();
}
void Triangle::calculateIntersectionConstVariables()
{
    v0_    = v_[2].v - v_[0].v;
    dot00_ = glm::dot(v0_, v0_);
    d_     = -1.f * glm::dot(normal_, v_[0].v);
}
std::optional<float> Triangle::intersect(const Ray& ray) const
{
    float t =
        -(normal_.x * ray.getPosition().x + normal_.y * ray.getPosition().y + normal_.z * ray.getPosition().z + d_) /
        (normal_.x * ray.getDirection().x + normal_.y * ray.getDirection().y + normal_.z * ray.getDirection().z);

    if (t < 0.001f)
    {
        return std::nullopt;
    }

    vec3 intersectPoint = ray.getPosition() + t * ray.getDirection();

    const auto& uv = calculateUV(intersectPoint);

    if (isInPlanePointInTriangle(uv))
    {
        return t;
    }

    return std::nullopt;
}

vec3 Triangle::getNormal(const vec3& intersectPoint) const
{
    if (useSmoothNormal_)
    {
        const auto& uv = calculateUV(intersectPoint);
        return calculateSmoothNormal(uv);
    }

    return normal_;
}

void Triangle::setVertexNormals(const vec3& vn1, const vec3& vn2, const vec3& vn3)
{
    v_[0].vn = vn1;
    v_[1].vn = vn2;
    v_[2].vn = vn3;
}

void Triangle::setUVs(const vec2& vt1, const vec2& vt2, const vec2& vt3)
{
    v_[0].vt = vt1;
    v_[1].vt = vt2;
    v_[2].vt = vt3;
}

void Triangle::enableSmoothNormals(bool use)
{
    useSmoothNormal_ = use;
}

bool Triangle::isInPlanePointInTriangle(const vec2& uv) const
{
    return (uv.x >= 0) && (uv.y >= 0) && (uv.x + uv.y < 1);
}

vec2 Triangle::calculateUV(const vec3& intersectPoint) const
{
    vec3 v2 = intersectPoint - v_[0].v;
    vec3 v1 = v_[1].v - v_[0].v;

    float dot01    = glm::dot(v0_, v1);
    float dot02    = glm::dot(v0_, v2);
    float dot11    = glm::dot(v1, v1);
    float dot12    = glm::dot(v1, v2);
    float invDenom = 1.f / (dot00_ * dot11 - dot01 * dot01);

    vec2 result;
    result.y = (dot11 * dot02 - dot01 * dot12) * invDenom;
    result.x = (dot00_ * dot12 - dot01 * dot02) * invDenom;

    return result;
}

vec3 Triangle::calculateStaticNormal(const vec3& v1, const vec3& v2, const vec3& v3)
{
    vec3 ab = v2 - v1;
    vec3 cb = v3 - v2;
    return glm::normalize(glm::cross(ab, cb));
}

vec3 Triangle::calculateSmoothNormal(const vec2& uv) const
{
    return glm::normalize((1.f - uv.x - uv.y) * v_[0].vn + uv.x * v_[1].vn + uv.y * v_[2].vn);
}
}  // namespace RayTracer
}  // namespace GameEngine
