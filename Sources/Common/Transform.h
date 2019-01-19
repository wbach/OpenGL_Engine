#pragma once
#include "Mutex.hpp"
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

    void TakeSnapShoot();
    const TransformContext& GetSnapShoot() const;
    void IncrasePosition(float dx, float dy, float dz, uint32 index = 0);
    void IncrasePosition(vec3 v, uint32 index = 0);
    void IncreaseRotation(float dx, float dy, float dz);

    vec3 GetPosition();
    vec3 GetRotation();
    vec3 GetScale();
    vec2 GetPositionXZ();
    const mat4& GetMatrix() const;

    void SetScale(float s);
    void SetScale(const vec3& s);
    void SetPosition(const vec3& pos);
    void SetYPosition(float);
    void SetPositionXZ(const vec2& pos);
    void SetRotation(const vec3& r);
    void SetRotate(Axis axis, float v);

public:
    bool isDynamic_;

private:
    void UpdateMatrix();

private:
    std::mutex contextMutex_;
    TransformContext context_;
    TransformContext snapshoot_;
    mat4 matrix;
};
}  // common
