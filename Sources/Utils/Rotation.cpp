#include "Rotation.h"

#include <iomanip>
namespace
{
vec3 EulerAngles(const Quaternion &quaternion)
{
    auto euler = glm::eulerAngles(quaternion);  // <-90, 90>

    if (std::fabs(euler.z) >= glm::half_pi<float>())
    {
        euler.x += glm::pi<float>();
        euler.y = glm::pi<float>() - euler.y;
        euler.z += glm::pi<float>();
    }

    return euler;
}
}  // namespace
Rotation::Rotation()
    : value_(RadiansVec3(0).value)
{
}

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
Rotation &Rotation::operator=(const Quaternion &v)
{
    value_ = v;
    return *this;
}

namespace std
{
std::string to_string(const Rotation &v)
{
    return std::to_string(v.value_);
}
void from_string(const std::string &s, Rotation &q)
{
    from_string(s, q.value_);
}
}  // namespace std

std::ostream &operator<<(std::ostream &os, const Rotation &r)
{
    auto degrees  = r.GetEulerDegrees();
    auto radians  = r.GetEulerRadians();
    const auto &q = r.value_;

    os << "--- Rotation Info ---\n";
    os << std::fixed << std::setprecision(3);
    os << "Quaternion: [" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << "]\n";
    os << "Degrees:    [" << degrees->x << "°, " << degrees->y << "°, " << degrees->z << "°]\n";
    os << "Radians:    [" << radians->x << "rad, " << radians->y << "rad, " << radians->z << "rad]\n";
    os << "---------------------";

    return os;
}
