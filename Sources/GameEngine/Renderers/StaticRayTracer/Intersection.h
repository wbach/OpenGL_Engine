#pragma once
#include <Types.h>

namespace GameEngine
{
namespace RayTracer
{
class Ray;
class IObject;

class Intersection
{
public:
    Intersection();

    void calculateIntersectionPointAndObjNormal(const Ray& ray, float t);
    const IObject* getObject() const;
    const vec3& getNormal() const;
    void setObject(IObject* ptr);
    const vec3& getPoint() const;
    operator bool() const
    {
        return isSet_;
    }

private:
    vec3 point_;
    IObject* objPtr_;
    bool isSet_;
    vec3 normal_;
};
}  // namespace RayTracer
}  // namespace GameEngine
