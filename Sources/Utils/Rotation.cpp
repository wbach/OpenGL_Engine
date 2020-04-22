#include "Rotation.h"

namespace
{
vec3 EulerAngles(const Quaternion &quaternion)
{
    auto euler = glm::eulerAngles(quaternion); // <-90, 90>

    if (std::fabs(euler.z) >= glm::pi<float>() / 2.f)
    {
        euler.x += glm::pi<float>();
        euler.y = glm::pi<float>() - euler.y;
        euler.z += glm::pi<float>();
    }

    return euler;
}
}  // namespace
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
    return RadiansVec3(EulerAngles(value_)).Degrees();
}

RadiansVec3 Rotation::GetEulerRadians() const
{
    return RadiansVec3(EulerAngles(value_));
}
