#pragma once
#include "Types.h"

struct Rotation
{
    Rotation();
    Rotation(const DegreesVec3& v);
    Rotation(const RadiansVec3& v);
    Rotation(const Quaternion& v);
    Rotation& operator=(const Quaternion&);

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

namespace std
{
std::string to_string(const Rotation&);
void from_string(const std::string&, Rotation&);
}

std::ostream& operator<<(std::ostream&, const Rotation&);
