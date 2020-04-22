#include "Camera.h"

#include <GLM/GLMUtils.h>
#include <Logger/Log.h>
#include <Utils.h>

#include <algorithm>

namespace GameEngine
{
namespace
{
const float NOTIF_EPSILON{std::numeric_limits<float>::epsilon()};
}  // namespace

BaseCamera::BaseCamera()
    : BaseCamera(0, 0)
{
}
BaseCamera::BaseCamera(float pitch, float yaw)
    : lock_(false)
    , position_(0.f)
    , rotation_(DegreesVec3(pitch, yaw, 0))
    , viewMatrix_(1.f)
    , lastNotifiedPosition_(0)
    , lastNotifRotation_(DegreesVec3(0.f))
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
    auto l1 = glm::length(position_ - lastNotifiedPosition_);
    auto l2 = glm::length(rotation_.value_ - lastNotifRotation_.value_);

    if (l1 > NOTIF_EPSILON or l2 > NOTIF_EPSILON)
    {
        for (auto& sub : subscribers_)
        {
            sub.second(*this);
        }
        lastNotifRotation_    = rotation_;
        lastNotifiedPosition_ = position_;
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
    auto direction   = position_ - lookAtPosition;
    auto yaw         = atan2f(direction.z, direction.x) - static_cast<float>(M_PI) / 2.f;
    auto pitch       = atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
    glm::quat qYaw   = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
    rotation_.value_ = qPitch * qYaw;
    NotifySubscribers();
}
const vec3& BaseCamera::GetDirection() const
{
    return direction_;
}
const vec3& BaseCamera::GetPosition() const
{
    return position_;
}
const Rotation &BaseCamera::GetRotation() const
{
    return rotation_;
}
float BaseCamera::GetPitch() const
{
    return rotation_.GetEulerDegrees()->x;
}
void BaseCamera::SetPitch(float angle)
{
    glm::quat pitch  = glm::angleAxis(glm::radians(angle), vec3(1.f, 0.f, 0.f));
    glm::quat yaw    = glm::angleAxis(glm::eulerAngles(rotation_.value_).y, vec3(0.f, 1.f, 0.f));
    rotation_.value_ = pitch * yaw;
    NotifySubscribers();
}
void BaseCamera::SetRotation(const Rotation& rotation)
{
    rotation_ = rotation;
    NotifySubscribers();
}
float BaseCamera::GetYaw() const
{
    return rotation_.GetEulerDegrees()->y;
}
void BaseCamera::SetYaw(float angle)
{
    glm::quat pitch  = glm::angleAxis(glm::eulerAngles(rotation_.value_).x, vec3(1.f, 0.f, 0.f));
    glm::quat yaw    = glm::angleAxis(glm::radians(angle), vec3(0.f, 1.f, 0.f));
    rotation_.value_ = pitch * yaw;
    NotifySubscribers();
}
void BaseCamera::CalculateDirection()
{
    float pitch_ = rotation_.GetEulerRadians()->x;
    float yaw_   = rotation_.GetEulerRadians()->y;
    float xzLen  = cosf(pitch_);

    direction_.z = xzLen * cosf(yaw_);
    direction_.y = sinf(pitch_);
    direction_.x = xzLen * sinf(-yaw_);
    direction_   = glm::normalize(direction_) * -1.f;
}
void BaseCamera::UpdateViewMatrix()
{
    viewMatrix_ = mat4(1.f);
    viewMatrix_ *= glm::mat4_cast(rotation_.value_);
    viewMatrix_ *= glm::translate(-position_);
}
const mat4& BaseCamera::GetViewMatrix() const
{
    return viewMatrix_;
}
void BaseCamera::IncreaseYaw(float yaw)
{
    rotation_.value_ *= glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
    NotifySubscribers();
}
void BaseCamera::IncreasePitch(float pitch)
{
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    rotation_.value_ = qPitch * rotation_.value_;
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
