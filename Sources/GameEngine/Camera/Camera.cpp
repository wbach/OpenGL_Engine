#include "Camera.h"
#include <glm/gtx/quaternion.hpp>
#include "GLM/GLMUtils.h"
#include "Utils.h"

namespace GameEngine
{
BaseCamera::BaseCamera()
    : BaseCamera(20, 0)
{
}
BaseCamera::BaseCamera(float pitch, float yaw)
    : up_(0, 1, 0)
    , position_(0)
    , rotation_(0)
    , viewMatrix_(mat4())
{
    UpdateMatrix();
}
BaseCamera::BaseCamera(const vec3& position, const vec3& lookAt)
    : position_(position)
{
    LookAt(lookAt);
}
void BaseCamera::Move()
{
}
void BaseCamera::CalculateInput()
{
}
void BaseCamera::CalculateZoom(float zoom_lvl)
{
}
void BaseCamera::UpdateMatrix()
{
    UpdateViewMatrix();
    CalculateDirection();
}
void BaseCamera::SetPosition(const vec3& position)
{
    position_ = position;
}
void BaseCamera::LookAt(const vec3& lookAtPosition)
{
    auto direction = position_ - lookAtPosition;
    rotation_.y    = Utils::ToDegrees(atan2(direction.z, direction.x) - static_cast<float>(M_PI) / 2.f);
    rotation_.x    = Utils::ToDegrees(atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z)));
}

void BaseCamera::InvertPitch()
{
    rotation_.x *= -1.f;
}
const vec3& BaseCamera::GetDirection() const
{
    return direction_;
}
const vec3& BaseCamera::GetPosition() const
{
    return position_;
}
const vec3& BaseCamera::GetRotation() const
{
    return rotation_;
}
float BaseCamera::GetPitch() const
{
    return rotation_.x;
}
void BaseCamera::SetPitch(float p)
{
    rotation_.x = p;
}
void BaseCamera::SetRotation(const vec3& rotation)
{
    rotation_ = rotation;
}
float BaseCamera::GetYaw() const
{
    return rotation_.y;
}
void BaseCamera::SetYaw(float y)
{
    rotation_.y = y;
}
float BaseCamera::GetRoll() const
{
    return rotation_.z;
}
void BaseCamera::SetRoll(float roll)
{
    rotation_.z = roll;
}
void BaseCamera::CalculateDirection()
{
    float pitch_ = Utils::ToRadians(rotation_.x);
    float yaw_   = Utils::ToRadians(rotation_.y);
    float xzLen  = cos(pitch_);

    direction_.z = xzLen * cos(yaw_);
    direction_.y = sin(pitch_);
    direction_.x = xzLen * sin(-yaw_);
    direction_   = glm::normalize(direction_) * -1.f;
}
void BaseCamera::UpdateViewMatrix()
{
    float p  = GetPitch();
    float y  = GetYaw();
    float r  = GetRoll();
    auto pos = GetPosition();

    viewMatrix_ = mat4(1.0f);
    viewMatrix_ *= glm::rotate(p, 1.0f, 0.0f, 0.0f);
    viewMatrix_ *= glm::rotate(y, 0.0f, 1.0f, 0.0f);
    viewMatrix_ *= glm::rotate(r, 0.0f, 0.0f, 1.0f);
    viewMatrix_ *= glm::translate(-pos);
}
const mat4& BaseCamera::GetViewMatrix() const
{
    return viewMatrix_;
}
}  // namespace GameEngine
