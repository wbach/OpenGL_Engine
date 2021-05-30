#include "Ray.h"

namespace GameEngine
{
namespace RayTracer
{
Ray::Ray(const vec3& pos, const vec3& dir, int x, int y)
    : position_(pos)
    , direction_(dir)
    , x_(x)
    , y_(y)
{
}
const vec3& Ray::getPosition() const
{
    return position_;
}
const vec3& Ray::getDirection() const
{
    return direction_;
}
int Ray::getX() const
{
    return x_;
}
int Ray::getY() const
{
    return y_;
}
}  // namespace RayTracer
}  // namespace GameEngine
