#include "Transform.h"

#include <GLM/GLMUtils.h>

#include <Mutex.hpp>
#include <algorithm>
#include <limits>

namespace common
{
namespace
{
std::mutex contextMutex;
}
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
    , matrix(Utils::CreateTransformationMatrix(pos, rot, scale))
    , idPool_(0)
{
}

Transform::Transform(const Transform& t)
    : isDynamic_(t.isDynamic_)
    , context_(t.context_)
    , snapshoot_(t.snapshoot_)
    , matrix(t.matrix)
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
    std::lock_guard<std::mutex> l(contextMutex);
    context_.position += v;
    NotifySubscribers();
}

void Transform::IncreaseRotation(float dx, float dy, float dz)
{
    std::lock_guard<std::mutex> l(contextMutex);
    context_.rotation += vec3(dx, dy, dz);
    NotifySubscribers();
}

void Transform::SetScale(float s)
{
    std::lock_guard<std::mutex> l(contextMutex);
    context_.scale = vec3(s);
    NotifySubscribers();
}

void Transform::SetScale(const vec3& s)
{
    std::lock_guard<std::mutex> l(contextMutex);
    context_.scale = s;
    NotifySubscribers();
}

void Transform::SetPosition(const vec3& pos)
{
    std::lock_guard<std::mutex> l(contextMutex);
    context_.position = pos;
    NotifySubscribers();
}

void Transform::SetYPosition(float pos)
{
    std::lock_guard<std::mutex> l(contextMutex);
    context_.position.y = pos;
    NotifySubscribers();
}

void Transform::SetPositionXZ(const vec2& pos)
{
    std::lock_guard<std::mutex> l(contextMutex);
    context_.position.x = pos.x;
    context_.position.z = pos.y;
    NotifySubscribers();
}

void Transform::SetRotation(const vec3& r)
{
    std::lock_guard<std::mutex> l(contextMutex);
    context_.rotation = r;
    NotifySubscribers();
}

void Transform::SetRotate(Axis axis, float v)
{
    std::lock_guard<std::mutex> l(contextMutex);
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

void Transform::TakeSnapShoot()
{
    std::lock_guard<std::mutex> l(contextMutex);
    snapshoot_ = context_;
    UpdateMatrix();
}

void Transform::UpdateMatrix()
{
    matrix = Utils::CreateTransformationMatrix(context_.position, context_.rotation, context_.scale);
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
    std::lock_guard<std::mutex> l(contextMutex);
    return vec2(context_.position.x, context_.position.z);
}

const mat4& Transform::GetMatrix() const
{
    return matrix;
}
}  // namespace common
