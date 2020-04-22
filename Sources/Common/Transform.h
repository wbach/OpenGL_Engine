#pragma once
#include <Mutex.hpp>
#include <functional>
#include <utility>
#include <Rotation.h>
#include "Types.h"

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
    vec3 position = vec3(0.f);
    Rotation rotation;
    vec3 scale = vec3(1.f);
};

class Transform
{
public:
    Transform();
    Transform(const vec2& pos);
    Transform(const vec3& pos);
    Transform(const vec3& pos, const DegreesVec3& rotation);
    Transform(const vec3& pos, const DegreesVec3& rotation, const vec3& scale);
    Transform(const Transform& transform);

    uint32 SubscribeOnChange(std::function<void(const Transform&)>);
    void UnsubscribeOnChange(uint32);

    void SetYPosition(float);
    void SetPosition(const vec3& pos);
    void SetPositionXZ(const vec2& pos);
    void IncrasePosition(const vec3& v);
    void IncrasePosition(float dx, float dy, float dz);
    vec2 GetPositionXZ() const;
    const vec3& GetPosition() const;

    void SetRotate(Axis axis, DegreesFloat v);
    void SetRotation(const DegreesVec3& eulerAngles);
    void SetRotation(const RadiansVec3& eulerAngles);
    void SetRotation(const Quaternion& rotation);
    void SetRotation(const Rotation& rotation);
    void SetPositionAndRotation(const vec3& position, const DegreesVec3& rotation);
    void SetPositionAndRotation(const vec3& position, const RadiansVec3& rotation);
    void SetPositionAndRotation(const vec3& position, const Quaternion& rotation);
    void SetPositionAndRotation(const vec3& position, const Rotation& rotation);
    void IncreaseRotation(const DegreesVec3&);
    void IncreaseRotation(const RadiansVec3&);

    vec3 TransformDirection(const vec3&) const;
    const Rotation& GetRotation() const;

    void SetScale(float s);
    void SetScale(const vec3& s);
    const vec3& GetScale() const;

    void TakeSnapShoot();
    const mat4& GetMatrix() const;
    const TransformContext& GetSnapShoot() const;

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
