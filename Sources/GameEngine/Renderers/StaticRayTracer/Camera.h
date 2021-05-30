#pragma once
#include <Types.h>

namespace GameEngine
{
namespace RayTracer
{
class Camera
{
public:
    Camera(const vec2ui& viewPort, float fov, const vec3& up, const vec3& lookAtPosition, const vec3& position);
    const mat3& getMatrix() const;
    const vec2ui& getViewPort() const;
    const vec3& getPosition() const;

private:
    void calculateMatrix();

private:
    vec3 up_;
    float fov_;
    vec2ui viewPort_;
    vec3 lookPostion_;
    vec3 eyePosition_;
    mat3 primaryMatrix_;
};
}  // namespace RayTracer
}  // namespace GameEngine
