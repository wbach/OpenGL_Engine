#include "Transform.h"

#include <GLM/GLMUtils.h>


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
    : Transform(pos, vec3(0))
{
}
Transform::Transform(const vec3& pos, const vec3& rot)
    : Transform(pos, rot, vec3(1))
{
}
Transform::Transform(const vec3& pos, const vec3& rot, const vec3& scale)
    : isDynamic_(false)
    , context_({pos, rot, scale})
    , snapshoot_({pos, rot, scale})
    , matrix_(Utils::CreateTransformationMatrix(pos, rot, scale))
    , idPool_(0)
{
}

Transform::Transform(const Transform& t)
    : isDynamic_(t.isDynamic_)
    , context_(t.context_)
    , snapshoot_(t.snapshoot_)
    , matrix_(t.matrix_)
    , subscribers_(t.subscribers_)
    , idPool_(t.idPool_)
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
    auto iter = std::find_if(subscribers_.begin(), subscribers_.end(), [id](auto& pair) { return pair.first == id; });
    if (iter != subscribers_.end())
        subscribers_.erase(iter);
}

void Transform::IncrasePosition(float dx, float dy, float dz, uint32 index)
{
    IncrasePosition(vec3(dx, dy, dz));
}

void Transform::IncrasePosition(vec3 v, uint32 index)
{
    context_.position += v;
    NotifySubscribers();
}

void Transform::IncreaseRotation(float dx, float dy, float dz)
{
    context_.rotation += vec3(dx, dy, dz);
    NotifySubscribers();
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

void Transform::SetPosition(const vec3& pos)
{
    context_.position = pos;
    NotifySubscribers();
}

void Transform::SetYPosition(float pos)
{
    context_.position.y = pos;
    NotifySubscribers();
}

void Transform::SetPositionXZ(const vec2& pos)
{
    context_.position.x = pos.x;
    context_.position.z = pos.y;
    NotifySubscribers();
}

void Transform::SetRotation(const vec3& r)
{
    context_.rotation = r;
    NotifySubscribers();
}

void Transform::SetRotate(Axis axis, float v)
{
    std::lock_guard<std::mutex> l(contextMutex_);
    switch (axis)
    {
        case X:
            context_.rotation.x = v;
            break;
        case Y:
            context_.rotation.y = v;
            break;
        case Z:
            context_.rotation.z = v;
            break;
        default:
            return;
    }
    NotifySubscribers();
}

void Transform::SetPositionAndRotation(const vec3& position, const vec3& rotation)
{
    context_.position = position;
    context_.rotation = rotation;
    NotifySubscribers();
}

void Transform::TakeSnapShoot()
{
    snapshoot_ = context_;
    UpdateMatrix();
}

void Transform::UpdateMatrix()
{
    matrix_ = Utils::CreateTransformationMatrix(snapshoot_.position, snapshoot_.rotation, snapshoot_.scale);
}

void Transform::NotifySubscribers()
{
    for (auto& sub : subscribers_)
    {
        sub.second(*this);
    }
}

const TransformContext& Transform::GetSnapShoot() const
{
    return snapshoot_;
}

const vec3& Transform::GetPosition() const
{
    return context_.position;
}

const vec3& Transform::GetRotation() const
{
    return context_.rotation;
}

const vec3& Transform::GetScale() const
{
    return context_.scale;
}

vec2 Transform::GetPositionXZ() const
{
    return vec2(context_.position.x, context_.position.z);
}

const mat4& Transform::GetMatrix() const
{
    return matrix_;
}
}  // namespace common
