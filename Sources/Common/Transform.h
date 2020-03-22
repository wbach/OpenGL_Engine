#pragma once
#include <Mutex.hpp>
#include "Types.h"
#include <utility>
#include <functional>

namespace common
{
enum Axis
{
    X = 0,
    Y,
    Z
};

struct TransformContext
{
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

class Transform
{
public:
    Transform();
    Transform(const vec2& pos);
    Transform(const vec3& pos);
    Transform(const vec3& pos, const vec3& rot);
    Transform(const vec3& pos, const vec3& rot, const vec3& scale);
    Transform(const Transform& transform);
    uint32 SubscribeOnChange(std::function<void(const Transform&)>);
    void UnsubscribeOnChange(uint32);

    void TakeSnapShoot();
    const TransformContext& GetSnapShoot() const;
    void IncrasePosition(float dx, float dy, float dz, uint32 index = 0);
    void IncrasePosition(vec3 v, uint32 index = 0);
    void IncreaseRotation(float dx, float dy, float dz);

    const vec3& GetPosition() const;
    const vec3& GetRotation() const;
    const vec3& GetScale() const;
    vec2 GetPositionXZ() const;
    const mat4& GetMatrix() const;

    void SetScale(float s);
    void SetScale(const vec3& s);
    void SetPosition(const vec3& pos);
    void SetYPosition(float);
    void SetPositionXZ(const vec2& pos);
    void SetRotation(const vec3& r);
    void SetRotate(Axis axis, float v);
    void SetPositionAndRotation(const vec3& position, const vec3& rotation);

public:
    bool isDynamic_;

private:
    void UpdateMatrix();
    void NotifySubscribers();

private:
    std::mutex contextMutex_;
    TransformContext context_;
    TransformContext snapshoot_;
    mat4 matrix_;

    uint32 idPool_;
    std::vector<std::pair<uint32, std::function<void(const Transform&)>>> subscribers_;
};
}  // namespace common
