#pragma once
#include <Types.h>

namespace GameEngine
{
namespace RayTracer
{
class Ray
{
public:
    Ray(const vec3& pos, const vec3& dir, int x, int y);
    const vec3& getPosition() const;
    const vec3& getDirection() const;
    int getX() const;
    int getY() const;

private:
    vec3 position_;
    vec3 direction_;
    float t_;
    int x_;
    int y_;
};
}  // namespace RayTracer
}  // namespace GameEngine
