#include "Transform.h"

#include <GLM/GLMUtils.h>
#include <Logger/Log.h>

#include <Utils/math.hpp>
#include <algorithm>
#include <limits>

namespace common
{
Transform::Transform()
    : Transform(vec3(0))
{
}
Transform::Transform(const vec2& pos)
    : Transform(vec3(pos.x, 0, pos.y))
{
}
Transform::Transform(const vec3& pos)
    : Transform(pos, DegreesVec3(0))
{
}
Transform::Transform(const vec3& pos, const DegreesVec3& degreesRotation)
    : Transform(pos, degreesRotation, vec3(1))
{
}
Transform::Transform(const vec3& pos, const DegreesVec3& degreesRotation, const vec3& scale)
    : context_({pos, Quaternion(degreesRotation.Radians()), scale})
    , snapshoot_(context_)
    , matrix_(Utils::CreateTransformationMatrix(context_.position, context_.rotation.value_, context_.scale))
    , idPool_(0)
{
}

Transform::Transform(const Transform& t)
    : context_(t.context_)
    , snapshoot_(t.snapshoot_)
    , matrix_(t.matrix_)
    , idPool_(t.idPool_)
    , subscribers_(t.subscribers_)
{
}

uint32 Transform::SubscribeOnChange(std::function<void(const Transform&)> callback)
{
    subscribers_.push_back({idPool_, callback});
    ++idPool_;
    return idPool_ - 1;
}

void Transform::UnsubscribeOnChange(uint32 id)
{
    if (not subscribers_.empty())
    {
        auto iter =
            std::find_if(subscribers_.begin(), subscribers_.end(), [id](auto& pair) { return pair.first == id; });

        if (iter != subscribers_.end())
        {
            std::lock_guard<std::mutex> lk(subscribeMutex_);
            subscribers_.erase(iter);
        }
    }
}

void Transform::SetYPosition(float pos)
{
    context_.position.y = pos;
    NotifySubscribers();
}

void Transform::SetPosition(const vec3& pos)
{
    context_.position = pos;
    NotifySubscribers();
}

void Transform::SetPositionXZ(const vec2& pos)
{
    context_.position.x = pos.x;
    context_.position.z = pos.y;
    NotifySubscribers();
}

void Transform::IncrasePosition(const vec3& v)
{
    context_.position += v;
    NotifySubscribers();
}

void Transform::IncrasePosition(float dx, float dy, float dz)
{
    context_.position.x += dx;
    context_.position.y += dy;
    context_.position.z += dz;
    NotifySubscribers();
}

vec2 Transform::GetPositionXZ() const
{
    return vec2(context_.position.x, context_.position.z);
}

const vec3& Transform::GetPosition() const
{
    return context_.position;
}

void Transform::SetRotate(Axis axis, DegreesFloat v)
{
    std::lock_guard<std::mutex> l(contextMutex_);

    auto euler = context_.rotation.GetEulerDegrees();

    switch (axis)
    {
        case X:
            euler.value.x = v.value;
            break;
        case Y:
            euler.value.y = v.value;
            break;
        case Z:
            euler.value.z = v.value;
            break;
    }

    SetRotation(euler);
    NotifySubscribers();
}

void Transform::SetRotation(const DegreesVec3& eulerAngles)
{
    SetRotation(Quaternion(eulerAngles.Radians()));
}

void Transform::SetRotation(const RadiansVec3& eulerAngles)
{
    SetRotation(Quaternion(eulerAngles.value));
}

void Transform::SetRotation(const Quaternion& rotation)
{
    context_.rotation = rotation;
    NotifySubscribers();
}

void Transform::SetRotation(const Rotation& rotation)
{
    context_.rotation = rotation;
    NotifySubscribers();
}

void Transform::SetPositionAndRotation(const vec3& position, const DegreesVec3& rotation)
{
    SetPositionAndRotation(position, Rotation(rotation));
}

void Transform::SetPositionAndRotation(const vec3& position, const RadiansVec3& rotation)
{
    SetPositionAndRotation(position, Rotation(rotation));
}

void Transform::SetPositionAndRotation(const vec3& position, const Quaternion& rotation)
{
    SetPositionAndRotation(position, Rotation(rotation));
}

void Transform::SetPositionAndRotation(const vec3& position, const Rotation& rotation)
{
    context_.position = position;
    context_.rotation = rotation;
    NotifySubscribers();
}

void Transform::SetPositionAndRotationAndScale(const vec3& position, const Rotation& rotation, const vec3& scale)
{
    context_.position = position;
    context_.rotation = rotation;
    context_.scale    = scale;
    NotifySubscribers();
}

void Transform::IncreaseRotation(const DegreesVec3& rotation)
{
    context_.rotation = context_.rotation.value_ * Quaternion(rotation.Radians());
    NotifySubscribers();
}

void Transform::IncreaseRotation(const RadiansVec3& rotation)
{
    context_.rotation = context_.rotation.value_ * Quaternion(rotation.value);
    NotifySubscribers();
}

vec3 Transform::TransformDirection(const vec3& v) const
{
    return context_.rotation.value_ * v;
}

const Rotation& Transform::GetRotation() const
{
    return context_.rotation;
}

void Transform::SetScale(float s)
{
    context_.scale = vec3(s);
    NotifySubscribers();
}

void Transform::SetScale(const vec3& s)
{
    context_.scale = s;
    NotifySubscribers();
}

const vec3& Transform::GetScale() const
{
    return context_.scale;
}

void Transform::TakeSnapShoot()
{
    snapshoot_ = context_;
    UpdateMatrix();
}

const mat4& Transform::GetMatrix() const
{
    return matrix_;
}

const TransformContext& Transform::GetSnapShoot() const
{
    return snapshoot_;
}

void Transform::UpdateMatrix()
{
    matrix_ = Utils::CreateTransformationMatrix(snapshoot_.position, snapshoot_.rotation.value_, snapshoot_.scale);
}

void Transform::NotifySubscribers()
{
    std::lock_guard<std::mutex> lk(subscribeMutex_);
    for (auto& sub : subscribers_)
    {
        sub.second(*this);
    }
}

void Transform::SetMatrix(const mat4& matrix)
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, rotation, translation, skew, perspective);
    SetPositionAndRotationAndScale(translation, rotation, scale);
}

}  // namespace common
