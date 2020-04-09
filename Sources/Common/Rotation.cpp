#include "Rotation.h"

namespace common
{
Rotation::Rotation(const DegreesVec3 &v)
    : value_(v.Radians())
{
}

Rotation::Rotation(const RadiansVec3 &v)
    : value_(v.value)
{
}

Rotation::Rotation(const Quaternion &v)
    : value_(v)
{
}

DegreesVec3 Rotation::GetEulerDegrees() const
{
    return RadiansVec3(glm::eulerAngles(value_)).Degrees();
}

RadiansVec3 Rotation::GetEulerRadians() const
{
    return RadiansVec3(glm::eulerAngles(value_));
}
}  // namespace common
