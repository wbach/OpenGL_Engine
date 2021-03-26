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

Camera::Camera()
    : Camera(0, 0)
{
}
Camera::Camera(float pitch, float yaw)
    : lock_(false)
    , idPool_(0)
    , position_(0.f)
    , rotation_(DegreesVec3(pitch, yaw, 0.f))
    , viewMatrix_(1.f)
    , lastNotifiedPosition_(0.f)
    , lastNotifRotation_(DegreesVec3(0.f))
{
    UpdateMatrix();
}
Camera::Camera(const vec3& position, const vec3& lookAt)
    : lock_(false)
    , position_(position)
    , rotation_(DegreesVec3(0.f, 0.f, 0.f))
    , viewMatrix_(1.f)
    , lastNotifiedPosition_(0.f)
    , lastNotifRotation_(DegreesVec3(0.f))
{
    LookAt(lookAt);
}
void Camera::Move()
{
}
void Camera::CalculateInput()
{
}
void Camera::CalculateZoom(float)
{
}
void Camera::Lock()
{
    lock_ = true;
}
void Camera::Unlock()
{
    lock_ = false;
}
bool Camera::IsLocked() const
{
    return lock_;
}
void Camera::UpdateMatrix()
{
    UpdateViewMatrix();
    CalculateDirection();
}
void Camera::SetPosition(const vec3& position)
{
    position_ = position;
    NotifySubscribers();
}
void Camera::NotifySubscribers()
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
IdType Camera::SubscribeOnChange(std::function<void(const ICamera&)> callback)
{
    auto id = idPool_.getId();
    subscribers_.push_back({id, callback});
    return id;
}
void Camera::UnsubscribeOnChange(IdType id)
{
    if (not subscribers_.empty())
    {
        auto iter =
            std::find_if(subscribers_.begin(), subscribers_.end(), [id](const auto& p) { return p.first == id; });

        if (iter != subscribers_.end())
        {
            subscribers_.erase(iter);
            idPool_.releaseId(id);
        }
    }
}
void Camera::LookAt(const vec3& lookAtPosition)
{
    rotation_.value_ = Utils::lookAt(lookAtPosition, position_);
    NotifySubscribers();
}
const vec3& Camera::GetDirection() const
{
    return direction_;
}
const mat4& Camera::GetTranslationMatrix() const
{
    return translationMatrix_;
}
const vec3& Camera::GetPosition() const
{
    return position_;
}
const Rotation& Camera::GetRotation() const
{
    return rotation_;
}
float Camera::GetPitch() const
{
    return rotation_.GetEulerDegrees()->x;
}
void Camera::SetPitch(float angle)
{
    glm::quat pitch  = glm::angleAxis(glm::radians(angle), vec3(1.f, 0.f, 0.f));
    glm::quat yaw    = glm::angleAxis(glm::yaw(rotation_.value_), vec3(0.f, 1.f, 0.f));
    rotation_.value_ = glm::normalize(pitch * yaw);
    NotifySubscribers();
}
void Camera::SetRotation(const Rotation& rotation)
{
    rotation_ = rotation;
    NotifySubscribers();
}
float Camera::GetYaw() const
{
    return rotation_.GetEulerDegrees()->y;
}
void Camera::SetYaw(float angle)
{
    glm::quat pitch  = glm::angleAxis(glm::pitch(rotation_.value_), vec3(1.f, 0.f, 0.f));
    glm::quat yaw    = glm::angleAxis(glm::radians(angle), vec3(0.f, 1.f, 0.f));
    rotation_.value_ = glm::normalize(pitch * yaw);
    NotifySubscribers();
}
void Camera::CalculateDirection()
{
    direction_ = glm::normalize(VECTOR_FORWARD * rotation_.value_);
}
void Camera::UpdateViewMatrix()
{
    rotationMatrix_    = glm::mat4_cast(rotation_.value_);
    translationMatrix_ = glm::translate(-position_);

    viewMatrix_ = rotationMatrix_ * translationMatrix_;
}
const mat4& Camera::GetRotationMatrix() const
{
    return rotationMatrix_;
}
const mat4& Camera::GetViewMatrix() const
{
    return viewMatrix_;
}
void Camera::IncreaseYaw(float yaw)
{
    rotation_.value_ *= glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));
    NotifySubscribers();
}
void Camera::IncreasePitch(float pitch)
{
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    rotation_.value_ = glm::normalize(qPitch * rotation_.value_);
    NotifySubscribers();
}
void Camera::IncreasePosition(const vec3& v)
{
    position_.x += v.x;
    position_.y += v.y;
    position_.z += v.z;
    NotifySubscribers();
}
void Camera::IncreasePositionX(float x)
{
    position_.x += x;
    NotifySubscribers();
}
void Camera::IncreasePositionY(float y)
{
    position_.y += y;
    NotifySubscribers();
}
void Camera::IncreasePositionZ(float z)
{
    position_.z += z;
    NotifySubscribers();
}
void Camera::IncreasePositionXZ(const vec2& v)
{
    position_.x += v.x;
    position_.z += v.y;
    NotifySubscribers();
}
}  // namespace GameEngine
