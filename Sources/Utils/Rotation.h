#pragma once
#include <Types.h>

struct Rotation
{
    Rotation();
    Rotation(const DegreesVec3& v);
    Rotation(const RadiansVec3& v);
    Rotation(const Quaternion& v);

    DegreesVec3 GetEulerDegrees() const;
    RadiansVec3 GetEulerRadians() const;

    Quaternion& operator*()
    {
        return value_;
    }
    Quaternion* operator->()
    {
        return &value_;
    }

    Quaternion value_;
};
