#include "Converter.h"

namespace GameEngine
{
namespace Physics
{
vec3 Convert(const btVector3& v)
{
    return vec3(v.getX(), v.getY(), v.getZ());
}

btVector3 Convert(const vec3& v)
{
    return btVector3(v.x, v.y, v.z);
}

btQuaternion Convert(const Quaternion& quat)
{
    return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

btTransform Convert(common::Transform& transform)
{
    btTransform result;
    result.setOrigin(Convert(transform.GetPosition()));
    Quaternion rotaton(transform.GetRotation());
    result.setRotation(Convert(rotaton));
    return result;
}
}  // namespace Physics
}  // namespace GameEngine