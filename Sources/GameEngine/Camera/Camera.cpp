#include "Camera.h"
#include <GLM/GLMUtils.h>
#include <Logger/Log.h>
#include <algorithm>
#include "Utils.h"

namespace GameEngine
{
BaseCamera::BaseCamera()
    : BaseCamera(0, 0)
{
}
BaseCamera::BaseCamera(float pitch, float yaw)
    : lock_(false)
    , up_(0, 1, 0)
    , position_(0)
    , rotation_(pitch, yaw, 0)
    , viewMatrix_(1.f)
{
    UpdateMatrix();
}
BaseCamera::BaseCamera(const vec3& position, const vec3& lookAt)
    : lock_(false)
    , position_(position)
{
    LookAt(lookAt);
}
void BaseCamera::Move()
{
}
void BaseCamera::CalculateInput()
{
}
void BaseCamera::CalculateZoom(float)
{
}

void BaseCamera::Lock()
{
    lock_ = true;
}

void BaseCamera::Unlock()
{
    lock_ = false;
}

bool BaseCamera::IsLocked() const
{
    return lock_;
}

void BaseCamera::UpdateMatrix()
{
    UpdateViewMatrix();
    CalculateDirection();
}
void BaseCamera::SetPosition(const vec3& position)
{
    position_ = position;
    NotifySubscribers();
}
void BaseCamera::NotifySubscribers()
{
    for (auto& sub : subscribers_)
    {
        sub.second(*this);
    }
}
uint32 BaseCamera::SubscribeOnChange(std::function<void(const ICamera&)> callback)
{
    subscribers_.push_back({idPool_, callback});
    ++idPool_;
    return idPool_ - 1;
}
void BaseCamera::UnsubscribeOnChange(uint32 id)
{
    auto iter = std::find_if(subscribers_.begin(), subscribers_.end(), [id](const auto& p) { return p.first == id; });

    if (iter != subscribers_.end())
    {
        subscribers_.erase(iter);
    }
}
void BaseCamera::LookAt(const vec3& lookAtPosition)
{
    auto direction = position_ - lookAtPosition;
    rotation_.y    = glm::degrees(atan2f(direction.z, direction.x) - static_cast<float>(M_PI) / 2.f);
    rotation_.x    = glm::degrees(atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z)));
    NotifySubscribers();
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
    NotifySubscribers();
}
void BaseCamera::SetRotation(const vec3& rotation)
{
    rotation_ = rotation;
    NotifySubscribers();
}
float BaseCamera::GetYaw() const
{
    return rotation_.y;
}
void BaseCamera::SetYaw(float y)
{
    rotation_.y = y;
    NotifySubscribers();
}
float BaseCamera::GetRoll() const
{
    return rotation_.z;
}
void BaseCamera::SetRoll(float roll)
{
    rotation_.z = roll;
    NotifySubscribers();
}
void BaseCamera::CalculateDirection()
{
    float pitch_ = glm::radians(rotation_.x);
    float yaw_   = glm::radians(rotation_.y);
    float xzLen  = cosf(pitch_);

    direction_.z = xzLen * cosf(yaw_);
    direction_.y = sinf(pitch_);
    direction_.x = xzLen * sinf(-yaw_);
    direction_   = glm::normalize(direction_) * -1.f;
}
void BaseCamera::UpdateViewMatrix()
{
    viewMatrix_ = mat4(1.f);
    viewMatrix_ *= glm::rotate(glm::radians(rotation_.x), vec3(1.0f, 0.0f, 0.0f));
    viewMatrix_ *= glm::rotate(glm::radians(rotation_.y), vec3(0.0f, 1.0f, 0.0f));
    viewMatrix_ *= glm::rotate(glm::radians(rotation_.z), vec3(0.0f, 0.0f, 1.0f));
    viewMatrix_ *= glm::translate(-position_);
}
const mat4& BaseCamera::GetViewMatrix() const
{
    return viewMatrix_;
}
void BaseCamera::IncreaseYaw(float yaw)
{
    rotation_.y += yaw;
    NotifySubscribers();
}
void BaseCamera::IncreasePitch(float pitch)
{
    rotation_.x += pitch;
    NotifySubscribers();
}
void BaseCamera::IncreasePosition(const vec3& v)
{
    position_.x += v.x;
    position_.y += v.y;
    position_.z += v.z;
    NotifySubscribers();
}
void BaseCamera::IncreasePositionX(float x)
{
    position_.x += x;
    NotifySubscribers();
}
void BaseCamera::IncreasePositionY(float y)
{
    position_.y += y;
    NotifySubscribers();
}
void BaseCamera::IncreasePositionZ(float z)
{
    position_.z += z;
    NotifySubscribers();
}
void BaseCamera::IncreasePositionXZ(const vec2& v)
{
    position_.x += v.x;
    position_.z += v.y;
    NotifySubscribers();
}
}  // namespace GameEngine
