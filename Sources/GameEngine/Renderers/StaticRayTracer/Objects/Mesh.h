#pragma once
#include <vector>
#include "GameEngine/Renderers/StaticRayTracer/Ray.h"
#include "Triangle.h"

namespace GameEngine
{
namespace RayTracer
{
class Mesh : public BaseObject
{
public:
    Mesh(const common::Transform&, const Material&);
    Mesh(const common::Transform&, const Material&, const std::vector<Triangle>&);
    virtual std::optional<float> intersect(const Ray&) const final;
    virtual vec3 getNormal(const vec3& intersectPoint) const;
    void addTriangle(const Triangle& triangle);

private:
    std::vector<Triangle> triangles_;
};
}  // namespace RayTracer
}  // namespace GameEngine
