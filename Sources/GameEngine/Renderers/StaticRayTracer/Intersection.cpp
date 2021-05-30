#include "Intersection.h"
#include "Objects/IObject.h"
#include "Ray.h"

namespace GameEngine
{
namespace RayTracer
{
Intersection::Intersection()
    : objPtr_(nullptr)
    , isSet_(false)
    , normal_(0)
{
}
void Intersection::calculateIntersectionPointAndObjNormal(const Ray& ray, float t)
{
    if (isSet_)
    {
        point_  = ray.getPosition() + t * ray.getDirection();
        normal_ = objPtr_->getNormal(point_);
    }
}
const IObject* Intersection::getObject() const
{
    return objPtr_;
}
const vec3& Intersection::getNormal() const
{
    return normal_;
}
void Intersection::setObject(IObject* ptr)
{
    objPtr_ = ptr;
    isSet_  = true;
}
const vec3& Intersection::getPoint() const
{
    return point_;
}
}  // namespace RayTracer
}  // namespace GameEngine
