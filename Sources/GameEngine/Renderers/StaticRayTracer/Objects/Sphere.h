#pragma once
#include "BaseObject.h"
#include "GameEngine/Renderers/StaticRayTracer/Ray.h"

namespace GameEngine
{
namespace RayTracer
{
class Sphere : public BaseObject
{
public:
    Sphere(const common::Transform&, const Material&, float);
    virtual std::optional<float> intersect(const Ray&) const final;
    virtual vec3 getNormal(const vec3& intersectPoint) const;

private:
    float radius_;
};
}  // namespace RayTracer
}  // namespace GameEngine
