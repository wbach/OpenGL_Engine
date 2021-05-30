#include "Camera.h"

namespace GameEngine
{
namespace RayTracer
{
Camera::Camera(const vec2ui& viewPort, float fov, const vec3& up, const vec3& lookAtPosition, const vec3& position)
    : up_(up)
    , fov_(fov)
    , viewPort_(viewPort)
    , lookPostion_(lookAtPosition)
    , eyePosition_(position)
{
    calculateMatrix();
}
const mat3& Camera::getMatrix() const
{
    return primaryMatrix_;
}
const vec2ui& Camera::getViewPort() const
{
    return viewPort_;
}
const vec3& Camera::getPosition() const
{
    return eyePosition_;
}
void Camera::calculateMatrix()
{
    vec3 lookVector = lookPostion_ - eyePosition_;

    vec3 u  = glm::normalize(glm::cross(up_, lookVector));
    vec3 v  = glm::normalize(glm::cross(lookVector, u));
    vec3 uf = u * (static_cast<float>(viewPort_.x) / 2.f);
    vec3 vf = v * (static_cast<float>(viewPort_.y) / 2.f);
    vec3 o  = glm::normalize(lookVector) * (viewPort_.x / (2.f * tanf(fov_ / 2.f))) - uf - vf;

    primaryMatrix_ = mat3(u.x, v.x, o.x, u.y, v.y, o.y, u.z, v.z, o.z);
}
}  // namespace RayTracer
}  // namespace GameEngine
