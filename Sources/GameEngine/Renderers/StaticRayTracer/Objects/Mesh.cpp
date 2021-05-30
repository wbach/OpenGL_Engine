#include "Mesh.h"
#include <memory>

namespace GameEngine
{
namespace RayTracer
{
Mesh::Mesh(const common::Transform& transform, const Material& material)
    : BaseObject(transform, material)
{
}
Mesh::Mesh(const common::Transform& transform, const Material& material, const std::vector<Triangle>& triangles)
    : BaseObject(transform, material)
    , triangles_(triangles)
{
}
void Mesh::addTriangle(const Triangle& triangle)
{
    triangles_.push_back(triangle);
}
std::optional<float> Mesh::intersect(const Ray& ray) const
{
    float t             = std::numeric_limits<float>::max();
    bool isIntersection = false;

    for (const auto& triangle : triangles_)
    {
        const auto& intersectionPoint = triangle.intersect(ray);

        if (intersectionPoint)
        {
            if (*intersectionPoint < t)
            {
                t              = *intersectionPoint;
                isIntersection = true;
            }
        }
    }

    if (isIntersection)
    {
        return t;
    }

    return std::nullopt;
}
vec3 Mesh::getNormal(const vec3&) const
{
    return vec3();
}
}  // namespace RayTracer
}  // namespace GameEngine
