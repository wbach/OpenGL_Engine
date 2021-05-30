#include "Sphere.h"
#include "GameEngine/Renderers/StaticRayTracer/Ray.h"

namespace GameEngine
{
namespace RayTracer
{
Sphere::Sphere(const common::Transform& transform, const Material& material, float radius)
    : BaseObject(transform, material)
    , radius_(radius)
{
}
std::optional<float> Sphere::intersect(const Ray& ray) const
{
    const auto& o = ray.getPosition();
    const auto& d = glm::normalize(ray.getDirection());
    const auto& c = transform_.GetPosition();
    auto v        = o - c;

    float A = glm::dot(d, d);
    float B = 2.f * glm::dot(v, d);
    float C = glm::dot(v, v) - radius_ * radius_;

    float t0 = (-B - sqrt(B * B - 4 * A * C)) / (2 * A);
    float t1 = (-B + sqrt(B * B - 4 * A * C)) / (2 * A);

    if (t0 > 0 && t1 > 0)
    {
        if (t0 > t1)
            return t1;
        else
            return t0;
    }

    return std::nullopt;
}

vec3 Sphere::getNormal(const vec3& intersectPoint) const
{
    return glm::normalize(intersectPoint - transform_.GetPosition());
}
}  // namespace RayTracer
}  // namespace GameEngine
